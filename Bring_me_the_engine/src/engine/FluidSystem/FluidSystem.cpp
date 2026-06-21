#include "engine/FluidSystem/FluidSystem.h"

void FluidSystem::init(int particleCount) {
    m_config = {
        .restDensity = 1000.0f,   // rho0
        .stiffness = 20.0f,       // On baisse la rigidité pour éviter les explosions au début
        .viscosity = 0.6f,        // Forte viscosité pour forcer l'amortissement
        .smoothingRadius = 0.05f, // h
        .particleMass = 1.0f,     // Valeur temporaire, sera écrasée par le init
        .gravity = -9.81f,

        .boxMin = Vec3(-2.0f, -0.98f, 6.0f),
        .boxMax = Vec3(0.0f, 2.0f, 8.0f)};

    float spawnSpacing = 0.035f;
    m_config.particleMass = m_buffer.init(particleCount, spawnSpacing, m_config);

    m_compute.init(m_buffer);
    m_FluidRenderer.init();

    m_FluidRenderer.setParticleRadius(spawnSpacing * 0.47f);
}

void FluidSystem::update(float dt) {
    m_compute.integrate(dt, m_config);

    // m_buffer.debugReadParticle(0, m_config);
}

void FluidSystem::render(const Camera & cam, const Mat4 & projection) {
    m_FluidRenderer.render(m_buffer, cam, projection);
}

void FluidSystem::setParams(const FluidConfig & config) {
    m_config = config;
}