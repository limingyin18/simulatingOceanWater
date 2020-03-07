#pragma once

#include "basic/MeshBasic.hpp"
#include "basic/Camera.hpp"
#include "basic/Shader.hpp"
#include "Ocean.hpp"

class OceanRender
{
public:
    OceanRender(Ocean& o, Camera& cam);
    void draw();

private:
    GLuint ebo, vbo, vao;
    Shader render;

    Camera& camera;
    Ocean& ocean;
};