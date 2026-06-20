#include "engine/FluidSystem/FluidSystem.h"

void FluidSystem::init(int particleCount) {
    m_config = {
        .restDensity = 1000.0f,
        .stiffness = 500.0f,
        .viscosity = 0.1f,
        .smoothingRadius = 0.6f,
        .particleMass = 42.0f,
        .gravity = -9.81f,

        .boxMin = Vec3(-2.0f, 0.0f, 6.0f),
        .boxMax = Vec3(2.0f, 10.0f, 10.0f)};

    float spawnSpacing = 0.25f;
    m_config.particleMass = m_buffer.init(particleCount, spawnSpacing, m_config);

    m_compute.init(m_buffer);
    m_FluidRenderer.init();
}

void FluidSystem::update(float dt) {
    m_compute.integrate(dt, m_config);

    m_buffer.debugReadParticle(0);
}

void FluidSystem::render(const Camera & cam, const Mat4 & projection) {
    m_FluidRenderer.render(m_buffer, cam, projection);
}

void FluidSystem::setParams(const FluidConfig & config) {
    m_config = config;
}