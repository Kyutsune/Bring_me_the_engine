#include "engine/FluidSystem/FluidSystem.h"

void FluidSystem::init(int particleCount) {
    m_config = {
        .restDensity = 1000.0f,   // rho0
        .stiffness = 20.0f,       // On baisse la rigidité pour éviter les explosions au début
        .viscosity = 0.6f,        // Forte viscosité pour forcer l'amortissement
        .smoothingRadius = 0.05f, // h
        .particleMass = 1.0f,     // Valeur temporaire, sera écrasée par le init
        .particleRadius = 0.0f,   // Rayon des particules
        .gravity = -9.81f,

        .boxMin = Vec3(-2.0f, -0.98f, 6.0f),
        .boxMax = Vec3(0.0f, 2.0f, 8.0f)};

    float spawnSpacing = 0.035f;
    m_config.particleMass = m_buffer.init(particleCount, spawnSpacing, m_config);
    m_config.particleRadius = spawnSpacing * 0.47f;

    m_compute.init(m_buffer);
    m_FluidRenderer.init();

    m_FluidRenderer.setParticleRadius(m_config.particleRadius);

    ObstacleData sphereObstacle;

    Vec3 boxCenter = (m_config.boxMin + m_config.boxMax) * 0.5f;
    boxCenter.y -= 1.2f;
    sphereObstacle.position = boxCenter;
    sphereObstacle.type = static_cast<int>(ObstacleType::SPHERE);
    sphereObstacle.size = Vec3(0.25f, 0.0f, 0.0f);
    m_obstacleBuffer.obstacles.push_back(sphereObstacle);

    glGenBuffers(1, &m_obstacleBuffer.ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_obstacleBuffer.ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ObstacleData) * m_obstacleBuffer.obstacles.size(), m_obstacleBuffer.obstacles.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void FluidSystem::update(float dt) {
    m_compute.integrate(dt, m_config, m_obstacleBuffer);

    // m_buffer.debugReadParticle(0, m_config);
}

void FluidSystem::render(const Camera & cam, const Mat4 & projection) {
    m_FluidRenderer.render(m_buffer, m_obstacleBuffer, cam, projection);
}

void FluidSystem::setParams(const FluidConfig & config) {
    m_config = config;
}