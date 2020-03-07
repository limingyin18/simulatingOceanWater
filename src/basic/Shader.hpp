#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>

class Shader
{
public:
    Shader();
    ~Shader();
    bool addShader(const std::string shaderFilename, const unsigned shaderType);
    bool addUniform(const std::string uniformName);
    bool link();

public:
    unsigned Program;
    std::vector<unsigned> shaders;
    std::unordered_map<std::string, int> uniforms;
};