#include "SceneBase.hpp"

using namespace std;
using namespace Eigen;

const string GLSL_VERSION = "#version 450";

SceneBase::SceneBase()
{
    initGLFW();
    initGLAD();
    initIMGUI();
}

SceneBase::~SceneBase()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

void SceneBase::draw()
{    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();

    cameraUpdate();
    update();

    drawIMGUI();

    glfwSwapBuffers(window);
}

void SceneBase::drawIMGUI()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        ImGui::Begin("info");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool SceneBase::shouldClose() const
{
    return glfwWindowShouldClose(window);
}

void SceneBase::initGLFW()
{
    // Init GLFW
    glfwInit();
    int major = *(GLSL_VERSION.cend()-3) - '0';
    int minor = *(GLSL_VERSION.cend()-2) - '0';
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create GLFWwindow
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSwapInterval(1);

    // Set callback function
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

void SceneBase::initGLAD()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glCullFace(GL_FRONT_AND_BACK);  
}

void SceneBase::initIMGUI()
{
    // create imgui
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION.c_str());
}

void SceneBase::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    SceneBase* obj = reinterpret_cast<SceneBase*>(glfwGetWindowUserPointer(window));
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            obj->keys[key] = true;
        else if (action == GLFW_RELEASE)
            obj->keys[key] = false;
    }
}

void SceneBase::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    SceneBase* obj = reinterpret_cast<SceneBase*>(glfwGetWindowUserPointer(window));
    std::swap(obj->mousePos, obj->mousePosPrev);
    obj->mousePos = {static_cast<float>(xpos), static_cast<float>(ypos)};
}

void SceneBase::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    SceneBase* obj = reinterpret_cast<SceneBase*>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS)
        obj->keys[button] = true;
    else if (action == GLFW_RELEASE)
        obj->keys[button] = false;
}

void SceneBase::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    SceneBase* obj = reinterpret_cast<SceneBase*>(glfwGetWindowUserPointer(window));
    obj->scrollOffset.first -= static_cast<float>(xoffset);
    obj->scrollOffset.second -= static_cast<float>(yoffset);
}

void SceneBase::cameraUpdate()
{
    if(keys[keyMap["alt"]])
    {
        pair<float, float> mouseOffset{(mousePos.second - mousePosPrev.second)/width, 
            float(mousePos.first - mousePosPrev.first)/height};

        if(keys[keyMap["mouseButtonLeft"]])
        {
            camera.rotation[0] += cameraRotateSpeed * mouseOffset.first;
            camera.rotation[1] += cameraRotateSpeed * mouseOffset.second;
            camera.rotation[0] = clamp(camera.rotation[0], -pi/2.0f, pi/2.0f);
        }

        if(keys[keyMap["mouseButtonMiddle"]])
        {
            camera.targetDistance *= (1.0f + cameraZoomSpeed* mouseOffset.first);
        }
    }

    camera.computeTransformation();
}