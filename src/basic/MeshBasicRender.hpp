#pragma once

#include "MeshBasic.hpp"
#include "Camera.hpp"
#include "Shader.hpp"

class PlaneRender
{
public:
    PlaneRender(Plane& p, Eigen::Matrix4f& model, Camera& cam);
    void draw();

private:
    GLuint ebo, vbo, vao;
    Shader render;

    Camera& camera;
    Eigen::Matrix4f& model;
    Plane& plane;
};