#pragma once

#include <string>
#include <exception>
#include <unordered_map>

#include <Eigen/Eigen>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "imguiGlfwOpenGL/imgui_impl_glfw.hpp"
#include "imguiGlfwOpenGL/imgui_impl_opengl3.hpp"

#include "Camera.hpp"

class SceneBase
{
public:
    explicit SceneBase();
    virtual ~SceneBase();

    void draw();
    void drawIMGUI();
    virtual void update() = 0;

    bool shouldClose() const;

    void initGLFW();
    void initGLAD();
    void initIMGUI();

private:
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

protected:
    void cameraUpdate();
    Camera camera;
    float cameraMoveSpeed = 1.0f;
    float cameraRotateSpeed = 1.0f;
    float cameraZoomSpeed = 1.0f;

    unsigned int width = 1280, height = 960;
    std::string title = "Hello Physics!";
    GLFWwindow* window;

    bool keys[1024] = {false};
    std::pair<float, float> mousePos = {0.0f, 0.0f};
    std::pair<float, float> mousePosPrev = {0.0f, 0.0f};
    std::pair<float, float> scrollOffset = {0.0f, 0.0f};
    std::unordered_map<std::string, int> keyMap =
        {{"alt", 342}, {"F", 70},
        {"mouseButtonLeft", 0}, {"mouseButtonRight", 1}, {"mouseButtonMiddle", 2}};
};