#include "Shader.hpp"

using namespace std;

Shader::Shader()
{
    Program = glCreateProgram();
}

Shader::~Shader()
{
    // delete shader source
    for(auto s:shaders)
    {
        glDeleteShader(s);
    }
}

bool Shader::addShader(const string shaderFilename, const unsigned shaderType)
{
    // read file to file stream
    ifstream shaderFile;
    shaderFile.open(shaderFilename);
    if (!shaderFile.is_open())
    {
        shaderFile.close();
        throw runtime_error("Can't open shader file.");
    }

    // convert to string stream
    stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    // convert to string
    string code;
    code = shaderStream.str();
    const char *code_c = code.c_str();

    // compile
    unsigned shader;
    int success;
    char infoLog[512];

    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &code_c, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        throw runtime_error(infoLog);
    }
    else
    {
        shaders.push_back(shader);
        return true;
    }
}

bool Shader::link()
{
    for(unsigned s:shaders)
    {
        glAttachShader(Program, s);
    }

    int success;
    char infoLog[512];
    glLinkProgram(Program);
    glGetProgramiv(Program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(Program, 512, NULL, infoLog);
        throw runtime_error(infoLog);
    }
    else
    {
        return true;
    }
}

bool Shader::addUniform(const std::string uniformName)
{
    int loc = glGetUniformLocation(Program, uniformName.c_str());
    if(loc==-1)
    {
        throw runtime_error(uniformName + "is not an active uniform name");
    }
    else
    {
        uniforms[uniformName] = loc;
        return true;
    }
}