#include "MeshBasic.hpp"

using namespace std;
using namespace Eigen;

void MeshBase::setVertices(function<void(unsigned, Vertex &)> const &fun)
{
    for (unsigned i = 0; i < data.size(); ++i)
    {
        fun(i, data[i]);
    }
}

void MeshBase::recomputeNormals(vector<Vertex> &data)
{
    for (auto &d : data)
    {
        d.normal = {0.f, 0.f, 0.f};
    }

    for (unsigned i = 0; i < indices.size();)
    {
        auto id1 = indices[i++];
        auto id2 = indices[i++];
        auto id3 = indices[i++];

        auto v1 = data[id1].position;
        auto v2 = data[id2].position;
        auto v3 = data[id3].position;

        // This does weighted area based on triangle area
        auto n = (v2 - v1).cross(v3 - v1);

        data[id1].normal = data[id1].normal + n;
        data[id2].normal = data[id2].normal + n;
        data[id3].normal = data[id3].normal + n;
    }

    for (auto &d : data)
    {
        d.normal.normalize();
    }
}

Plane::Plane(unsigned nx, unsigned nz)
{
    data.reserve((nx) * (nz));
    indices.reserve(6 * (nx - 1) * (nz - 1));

    const float dx = 2.0f / (nx - 1);
    const float dz = 2.0f / (nz - 1);

    for (unsigned i = 0; i < nx; ++i)
    {
        for (unsigned j = 0; j < nz; ++j)
        {
            data.emplace_back(Vector3f{-1.0f + i * dx, 0.0f, -1.0f + j * dz},
                              Vector3f{0.0f, 1.0f, 0.0f},
                              Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        }
    }

    for (unsigned i = 0; i < nx - 1; ++i)
    {
        for (unsigned j = 0; j < nz - 1; ++j)
        {
            const unsigned I = i * nz + j;
            indices.emplace_back(I);
            indices.emplace_back(I + 1);
            indices.emplace_back(I + nz);

            indices.emplace_back(I + nz);
            indices.emplace_back(I + 1);
            indices.emplace_back(I + nz + 1);
        }
    }
}

Cube::Cube(int n)
{
    data.reserve(6 * n * n);
    indices.reserve(6 * 6 * (n - 1) * (n - 1));

    const float dx = 2.0f / (n - 1);

    // bottom
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            data.emplace_back(Vector3f{-1.0f + i * dx, 1.0f, -1.0f + j * dx},
                              Vector3f{0.0f, -1.0f, 0.0f},
                              Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        }
    }

    // top
    for (int i = 0; i < n; ++i)
    {
        for (int j = n-1; j >= 0; --j)
        {
            data.emplace_back(Vector3f{-1.0f + i * dx, -1.0f, -1.0f + j * dx},
                              Vector3f{0.0f, 1.0f, 0.0f},
                              Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        }
    }

    // back
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            data.emplace_back(Vector3f{-1.0f + i * dx, -1.0f + j * dx, -1.0f},
                              Vector3f{0.0f, 0.0f, 1.0f},
                              Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        }
    }

    // front
    for (int i = 0; i < n; ++i)
    {
        for (int j = n-1; j >= 0; --j)
        {
            data.emplace_back(Vector3f{-1.0f + i * dx, -1.0f + j * dx, 1.0f},
                              Vector3f{0.0f, 0.0f, -1.0f},
                              Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        }
    }

    // left
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            data.emplace_back(Vector3f{-1.0f, -1.0f + i * dx, -1.0f + j * dx},
                              Vector3f{-1.0f, 0.0f, 0.0f},
                              Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        }
    }

    // right
    for (int i = 0; i < n; ++i)
    {
        for (int j = n-1; j >= 0; --j)
        {
            data.emplace_back(Vector3f{1.0f, -1.0f + i * dx, -1.0f + j * dx},
                              Vector3f{1.0f, 0.0f, 0.0f},
                              Vector4f{1.0f, 1.0f, 1.0f, 1.0f});
        }
    }

    // indices
    for (int k = 0; k < 6; ++k)
    {
        for (int i = 0; i < n - 1; ++i)
        {
            for (int j = 0; j < n - 1; ++j)
            {
                const unsigned I = i * n + j + k * n * n;
                indices.emplace_back(I);
                indices.emplace_back(I + 1);
                indices.emplace_back(I + n);

                indices.emplace_back(I + n);
                indices.emplace_back(I + 1);
                indices.emplace_back(I + n + 1);
            }
        }
    }
}
