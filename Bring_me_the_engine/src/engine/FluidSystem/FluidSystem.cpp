#include "engine/FluidSystem/FluidSystem.h"

void FluidSystem::init(int particleCount) {
    m_config = {
        .numberOfParticles = particleCount, // Nombre de particules dans le système
        .baseSpacing = 0.035f,              // Espacement initial entre les particules
        .restDensity = 1000.0f,             // rho0
        .stiffness = 20.0f,                 // On baisse la rigidité pour éviter les explosions au début
        .viscosity = 10.0f,                 // Forte viscosité pour forcer l'amortissement
        .smoothingRadius = 0.05f,           // h
        .particleMass = 1.0f,               // Valeur temporaire, sera écrasée par le init
        .particleRadius = 0.016f,           // Rayon des particules
        .gravity = -9.81f,

        .useBox = true,
        .boxMin = Vec3(-2.0f, 0.0f, 6.0f),
        .boxMax = Vec3(0.0f, 2.0f, 8.0f)};

    m_config.particleMass = m_buffer.init(particleCount, m_config.baseSpacing, m_config);

    m_compute.init(m_buffer);
    m_FluidRenderer.init();
    m_FluidRenderer.setParticleRadius(m_config.particleRadius);

    glGenBuffers(1, &m_obstacleBuffer.ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_obstacleBuffer.ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ObstacleData) * 16, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void FluidSystem::reset(int particleCount) {
    m_config.smoothingRadius = m_config.baseSpacing * 1.5f;

    // Relance l'init du buffer avec des proportions saines
    m_config.particleMass = m_buffer.init(particleCount, m_config.baseSpacing, m_config);

    m_FluidRenderer.setParticleRadius(m_config.particleRadius);

    std::cout << "[Reset] Nouveau h : " << m_config.smoothingRadius
              << " | Masse calibrée : " << m_config.particleMass << "\n";
}

void FluidSystem::update(float dt, const Scene & scene) {
    m_obstacleBuffer.obstacles.clear();
    int currentTextureSlot = 0;

    for (const auto & entity : scene.getEntities()) {
        if (entity->hasSDF()) {
            ObstacleData obs;
            obs.inverseModelMatrix = entity->getTransform().inverse();

            // CORRECTION : On lit les bornes du volume 3D, pas celles du mesh plat !
            auto sdfVolume = entity->getSDFVolume();
            obs.boxMin = sdfVolume->getLocalMin();
            obs.boxMax = sdfVolume->getLocalMax();

            obs.textureSlot = currentTextureSlot;
            obs.padding = 0.0f;

            m_obstacleBuffer.obstacles.push_back(obs);

            glActiveTexture(GL_TEXTURE4 + currentTextureSlot);
            glBindTexture(GL_TEXTURE_3D, sdfVolume->getTextureID());

            currentTextureSlot++;
            if (currentTextureSlot >= 8)
                break;
        }
    }

    // On met à jour le SSBO avec le nombre exact d'obstacles actifs cette frame
    if (!m_obstacleBuffer.obstacles.empty()) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_obstacleBuffer.ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(ObstacleData) * m_obstacleBuffer.obstacles.size(), m_obstacleBuffer.obstacles.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    // Envoi à la pipeline de compute
    m_compute.integrate(dt, m_config, m_obstacleBuffer);

    // m_buffer.debugReadParticle(0, m_config);
}

void FluidSystem::render(const Camera & cam, const Mat4 & projection) {
    m_FluidRenderer.render(m_buffer, m_obstacleBuffer, cam, projection);
}

void FluidSystem::setParams(const FluidConfig & config) {
    m_config = config;
}