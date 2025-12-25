#include "rendering/ShadowManager.h"
#include "Globals.h"

ShadowManager::ShadowManager(Shader * dirShadowShader, Shader * pointShadowShader)
    : m_dirShadowShader(dirShadowShader), m_pointShadowShader(pointShadowShader) {}

void ShadowManager::init_directionnal_shadows() {
    m_directionalShadow.init();
}

void ShadowManager::renderShadows(const Scene & scene) {
    // Shadow map directionnelle
    m_directionalShadow.render(scene, *m_dirShadowShader);

    // Shadow maps cubemap ponctuelles
    const auto & pointLights = scene.getLightingManager().getPonctualLights();

    // Resize si besoin
    if (m_punctualShadows.size() < pointLights.size()) {
        size_t oldSize = m_punctualShadows.size();
        m_punctualShadows.resize(pointLights.size());
        for (size_t i = oldSize; i < pointLights.size(); ++i)
            m_punctualShadows[i] = PonctualShadowMap(1024, 1024);
    }

    // Rendu de chaque light active
    for (size_t i = 0; i < pointLights.size(); ++i) {
        if (!pointLights[i].isActive())
            continue;
        m_punctualShadows[i].render(scene, *m_pointShadowShader, pointLights[i]);
    }
}

void ShadowManager::bindShadows(Shader & shader, const Scene & scene) {
    const Light & dirLight = scene.getLightingManager().getFirstDirectional();

    if (dirLight.getType() != LightType::LIGHT_ERROR && dirLight.isActive()) {
        shader.set("lightSpaceMatrix", m_directionalShadow.getLightSpaceMatrix(), false);
        shader.set("dirLightDirection", dirLight.getDirection());

        m_directionalShadow.bindTexture(GL_TEXTURE4);
        shader.set("shadowMap", 4);
        shader.set("useDirectionalShadow", true);
    } else {
        shader.set("useDirectionalShadow", false);
    }

    shader.set("usePointShadow", m_punctualShadowEnabled);

    const auto & pointLights = scene.getLightingManager().getPonctualLights();
    size_t realCount = std::min(m_punctualShadows.size(), pointLights.size());

    size_t count = std::min(realCount, MAX_PONC_LIGHTS);

    shader.set("pointLightNumber", static_cast<int>(count));

    // Bind les shadow maps actives (jusqu'à count)
    for (size_t i = 0; i < count; ++i) {
        m_punctualShadows[i].bindTexture(GL_TEXTURE5 + i);
    }
    // Si il y a moins que MAX_PONC_LIGHTS, il faut binder les autres
    // Donc on bind de count à max_ponc_lights des textures nulles pour avoir le tableau
    // complet sinon le rendu du shader sera bizarre
    for (size_t i = count; i < MAX_PONC_LIGHTS; ++i) {
        glActiveTexture(GL_TEXTURE5 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    // Préparer un tableau complet de MAX_PONC_LIGHTS unités de texture
    std::vector<GLint> units(MAX_PONC_LIGHTS);
    for (size_t i = 0; i < MAX_PONC_LIGHTS; ++i) {
        units[i] = 5 + static_cast<GLint>(i);
    }
    shader.setArray("pointShadowMaps", units.data(), static_cast<int>(MAX_PONC_LIGHTS));

    // Préparer et passer les autres uniformes en remplissant avec des valeurs neutres pour le reste
    for (size_t i = 0; i < MAX_PONC_LIGHTS; ++i) {
        if (i < count) {
            shader.set("pointLightPositions[" + std::to_string(i) + "]", pointLights[i].getPosition());
            shader.set("pointLightFarPlanes[" + std::to_string(i) + "]", pointLights[i].computeEffectiveRange(0.01f));
            shader.set("pointLightIntensities[" + std::to_string(i) + "]", pointLights[i].getIntensity());
        } else {
            shader.set("pointLightPositions[" + std::to_string(i) + "]", Vec3(0.0f));
            shader.set("pointLightFarPlanes[" + std::to_string(i) + "]", 1.0f);
            shader.set("pointLightIntensities[" + std::to_string(i) + "]", 0.0f);
        }
    }
}