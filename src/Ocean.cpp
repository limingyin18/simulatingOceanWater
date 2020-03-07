#include "Ocean.hpp"

using namespace std;

constexpr float EPSILON = 1e-6f;
constexpr float g = 9.8f;

Ocean::Ocean(int n, int m, float lx, float lz, float a, float v,
             complex<float> w, float l)
    : N(n), M(m), Lx(lx), Lz(lz), A(a), wind_speed(v), wind_dir(w), lambda(l),
      h_tlide_0(n * m, 0.),
      h_tlide_0_conj(n * m, 0.),
      normal_dist(0., 1.),
      plane(n, m)
{
    plane.setVertices([](unsigned i, MeshBase::Vertex& v){v.position *=100.f; v.color = Eigen::Vector4f{0.39f, 0.58f, 0.93f, 1.0f};});

    rand_generator.seed(static_cast<unsigned>(time(nullptr)));
    wind_dir /= abs(wind_dir);

    for (int m = 0; m < M; ++m)
    {
        for (int n = 0; n < N; ++n)
        {
            h_tlide_0[pos2idx(n, m)] = hTlide0(getK(n, m));
            h_tlide_0_conj[pos2idx(n, m)] = conj(hTlide0(-getK(n, m)));
        }
    }
    // create ssbo
    glCreateBuffers(1, &ssboAmpl);
    glNamedBufferStorage(ssboAmpl, N*N*2*4, nullptr, GL_MAP_READ_BIT);
    glCreateBuffers(1, &ssboNormalX);
    glNamedBufferStorage(ssboNormalX, N*N*2*4, nullptr, GL_MAP_READ_BIT);
    glCreateBuffers(1, &ssboNormalZ);
    glNamedBufferStorage(ssboNormalZ, N*N*2*4, nullptr, GL_MAP_READ_BIT);
    glCreateBuffers(1, &ssboDx);
    glNamedBufferStorage(ssboDx, N*N*2*4, nullptr, GL_MAP_READ_BIT);
    glCreateBuffers(1, &ssboDz);
    glNamedBufferStorage(ssboDz, N*N*2*4, nullptr, GL_MAP_READ_BIT);

    glCreateBuffers(1, &ssboHTilde);
    glNamedBufferStorage(ssboHTilde, N*N*2*4, h_tlide_0.data(), GL_MAP_READ_BIT);
    glCreateBuffers(1, &ssboHTildeConj);
    glNamedBufferStorage(ssboHTildeConj, N*N*2*4, h_tlide_0_conj.data(), GL_MAP_READ_BIT);

    // init ampl shader
	shaderAmpl.addShader("ampl.comp", GL_COMPUTE_SHADER);
	shaderAmpl.link();
	shaderAmpl.addUniform("N");
	shaderAmpl.addUniform("M");
	shaderAmpl.addUniform("Lx");
	shaderAmpl.addUniform("Lz");
	shaderAmpl.addUniform("t");
	glProgramUniform1i(shaderAmpl.Program, shaderAmpl.uniforms.at("N"), N);
	glProgramUniform1i(shaderAmpl.Program, shaderAmpl.uniforms.at("M"), M);
	glProgramUniform1f(shaderAmpl.Program, shaderAmpl.uniforms.at("Lx"), Lx);
	glProgramUniform1f(shaderAmpl.Program, shaderAmpl.uniforms.at("Lz"), Lz);

    // init fft2 shader
	shaderFFT2.addShader("ifft2.comp", GL_COMPUTE_SHADER);
	shaderFFT2.link();
	shaderFFT2.addUniform("N");
	glProgramUniform1i(shaderFFT2.Program, shaderFFT2.uniforms.at("N"), N);

    // init sign shader
	shaderSign.addShader("sign.comp", GL_COMPUTE_SHADER);
	shaderSign.link();
	shaderSign.addUniform("N");
	glProgramUniform1i(shaderSign.Program, shaderSign.uniforms.at("N"), N);
}

int Ocean::pos2idx(int n, int m) const
{
    return m * N + n;
}

complex<float> Ocean::getK(int n, int m) const
{
    return {2.f * pi * (static_cast<float>(n) - static_cast<float>(N) / 2.f) / Lx,
            2.f * pi * (static_cast<float>(m) - static_cast<float>(M) / 2.f) / Lz};
}

float Ocean::Ph(complex<float> k) const
{
    float k_abs = abs(k);
    if (k_abs < EPSILON)
        return 0.0f;

    // Phillips spectrum
    float L = pow(wind_speed, 2) / g;
    complex<float> k_hat = k / k_abs;
    float k_hatDotwind_dir = k_hat.real() * wind_dir.real() + k_hat.imag() * wind_dir.imag();
    float P = A * expf(-1.f / (pow(k_abs * L, 2))) / pow(k_abs, 4) * pow(k_hatDotwind_dir, 2);
    float l = 1.f; // suppress
    P *= expf(-k_abs * k_abs * l * l);

    return P;
}

std::complex<float> Ocean::hTlide0(complex<float> k)
{
    float e_r = normal_dist(rand_generator);
    float e_i = normal_dist(rand_generator);

    return sqrtf(0.5f) * std::complex<float>{e_r, e_i} * sqrtf(Ph(k));
}

std::complex<float> Ocean::hTlide(int n, int m, float t) const
{
    float k_norm = abs(getK(n, m));
    float omega = sqrtf(g*k_norm);
    complex<float> c = exp(complex<float>(0.0f, omega * t));

    return h_tlide_0[pos2idx(n, m)] * c + h_tlide_0_conj[pos2idx(n, m)] * conj(c);
}

void Ocean::update(float t)
{
    // ampl
    glUseProgram(shaderAmpl.Program);
    glUniform1f(shaderAmpl.uniforms.at("t"), t);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboAmpl);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboNormalX);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboNormalZ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssboDx);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssboDz);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ssboHTilde);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, ssboHTildeConj);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // ifft2
    glUseProgram(shaderFFT2.Program);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboAmpl);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboNormalX);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboNormalZ);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboDx);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboDz);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // sign
    glUseProgram(shaderSign.Program);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboAmpl);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboNormalX);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboNormalZ);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboDx);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboDz);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}