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

    glGenBuffers(1, &m_obstacleBuffer.ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_obstacleBuffer.ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ObstacleData) * 16, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void FluidSystem::update(float dt, const Scene & scene) {
    m_obstacleBuffer.obstacles.clear();
    int currentTextureSlot = 0;

    // Récupération dynamique des entités physiques de la scène
    for (const auto & entity : scene.getEntities()) {
        if (entity->hasSDF()) {
            ObstacleData obs;
            
            // Calcul indispensable : on donne au GPU de quoi annuler les transformations de l'objet
            obs.inverseModelMatrix = entity->getTransform().inverse();
            
            // On récupère la bounding box locale non transformée (celle utilisée par l'SDFGenerator)
            // Attention : l'SDFGenerator applique une marge de 10% (padding), 
            // il faut donc récupérer les mêmes bornes que ton SDFGenerator !
            AABB localBox = entity->getBoundingBox();
            Vec3 size = localBox.m_max - localBox.m_min;
            obs.boxMin = localBox.m_min - size * 0.1f;
            obs.boxMax = localBox.m_max + size * 0.1f;
            
            obs.textureSlot = currentTextureSlot;
            obs.padding = 0.0f;

            m_obstacleBuffer.obstacles.push_back(obs);

            // Liaison de la texture 3D sur l'unité de texture correspondante
            // On réserve les premières unités de textures aux ombres/GBuffer, et on binde les SDF à partir de GL_TEXTURE4
            glActiveTexture(GL_TEXTURE4 + currentTextureSlot);
            glBindTexture(GL_TEXTURE_3D, entity->getSDFVolume()->getTextureID());

            currentTextureSlot++;
            if (currentTextureSlot >= 8) break; // Sécurité pour ne pas dépasser la taille du tableau dans le shader
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