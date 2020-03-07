#pragma once

#include <vector>
#include <array>
#include <functional>
#include <Eigen/Eigen>

class MeshBase
{
public:
    struct Vertex
    {
        Eigen::Vector3f position;
        Eigen::Vector3f normal;
        Eigen::Vector4f color;

        explicit Vertex(Eigen::Vector3f p = {0.0f, 0.0f, 0.0f},
                            Eigen::Vector3f n = {0.0f, 0.0f, 0.0f},
                            Eigen::Vector4f c = {1.0f, 1.0f, 1.0f, 1.0f}) :
                            position{p}, normal{n}, color{c} {};
    };

    std::vector<Vertex> data;
    std::vector<unsigned> indices;

    void setVertices(std::function<void(unsigned, Vertex &)> const &fun);
    virtual void recomputeNormals(std::vector<Vertex> &data);
};

class Plane : public MeshBase
{
public:
    explicit Plane(unsigned n, unsigned m);
};

class Cube : public MeshBase
{
public:
    explicit Cube(int n);
};