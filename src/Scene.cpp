#include "Scene.hpp"

using namespace std;
using namespace Eigen;

constexpr int N = 32;
constexpr int M = 32;
constexpr float Lx = 100.0f;
constexpr float Lz = 100.0f;
constexpr float AMPLITUDE = 100.f;
constexpr float WIND_SPEED = 100.0f;
constexpr complex<float> WIND_DIRECTION = {1.f, 1.f};
constexpr float CHOPPY_FACTOR = 10.0f;

Scene::Scene() : SceneBase(),
ocean{N, M, Lx, Lz, AMPLITUDE, WIND_SPEED, WIND_DIRECTION, CHOPPY_FACTOR},
oceanRender{ocean, camera}
{
	m_time = chrono::system_clock::now();
}

Scene::~Scene()
{
}

void Scene::update()
{
    auto timeNow = chrono::system_clock::now();
    std::chrono::duration<float> elapsed =  timeNow - m_time;
    float dt = elapsed.count();
	total_time += dt;
    m_time = timeNow;

	physicsUpdate(dt);
	graphicsUpdate(dt);
}

void Scene::physicsUpdate(const float dt)
{
	ocean.update(total_time);
}

void Scene::graphicsUpdate(const float dt)
{
	oceanRender.draw();
}