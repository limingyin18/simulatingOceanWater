#pragma once

#include <chrono>
#include <array>
#include <random>
#include "basic/SceneBase.hpp"
#include "basic/MeshBasic.hpp"
#include "basic/Shader.hpp"
#include "basic/MeshBasicRender.hpp"

#include "Ocean.hpp"
#include "OceanRender.hpp"

class Scene : public SceneBase
{
public:
    explicit Scene();
    ~Scene();

    void update() override;

    void physicsUpdate(const float dt);
    void graphicsUpdate(const float dt);

private:
    std::chrono::time_point<std::chrono::system_clock> m_time;
    float total_time = 0.0f;

    Ocean ocean;
    OceanRender oceanRender;
};