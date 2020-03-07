#include "Camera.hpp"

using namespace Eigen;

Camera::Camera()
{
    computeTransformation();
}

void Camera::computeTransformation()
{
    assert(aspect != 0);
    assert(far != near);

    projection << 1 / (aspect * std::tan(fov / 2)), 0, 0, 0,
            0, 1 / std::tan(fov / 2), 0, 0,
            0, 0, -(far + near) / (far - near), -2 * far * near / (far - near),
            0, 0, -1, 0;

    view = (Translation<float,3>(Vector3f{0.0f, 0.f, -targetDistance}) * 
        AngleAxis<float>(-rotation[1], Vector3f(0.0f, 1.0f, 0.0f)) *
        AngleAxis<float>(-rotation[0], Vector3f(1.0f, 0.0f, 0.0f)) *
        Translation<float,3>(-target)).matrix();
}

Vector3f Camera::getCameraPos() const
{
    auto targetRot =  AngleAxis<float>(rotation[1], Vector3f(0.0f, 1.0f, 0.0f))*
                         AngleAxis<float>(rotation[0], Vector3f(1.0f, 0.0f, 0.0f));
    auto targetDistRot =  targetRot * Vector3f{0.0f, 0.f, targetDistance};
    return target + targetDistRot;
}