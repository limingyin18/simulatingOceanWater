#pragma once

#include <complex>
#include <ctime>
#include <random>
#include <vector>

#include "basic/Shader.hpp"
#include "basic/MeshBasic.hpp"
#include "basic/Math.hpp"

class Ocean
{
public:
    Ocean(int n, int m, float lx, float lz, float a, float v, std::complex<float> w, float l);
    void update(float t);

    Plane plane;
    GLuint ssboAmpl, ssboNormalX, ssboNormalZ, ssboDx, ssboDz;
    float lambda;                 // choppy factor

private:
    int pos2idx(int n, int m) const;
    std::complex<float> getK(int n, int m) const;

    float dispersion(float k) const;
    float Ph(std::complex<float> k) const;
    std::complex<float> hTlide0(std::complex<float> k);
    std::complex<float> hTlide(int n, int m, float t) const;

    int N, M;                     // sampling points
    float Lx, Lz;                 // patch size
    float A;                      // amplitude
    float wind_speed;             // wind seed
    std::complex<float> wind_dir; // wind direction

    std::default_random_engine rand_generator;
    std::normal_distribution<float> normal_dist;

    std::vector<std::complex<float>> h_tlide_0;
    std::vector<std::complex<float>> h_tlide_0_conj;

    Shader shaderAmpl;
    Shader shaderFFT2, shaderSign;
    GLuint ssboHTilde, ssboHTildeConj;
};