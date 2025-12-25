#include "engine/LightingManager.h"
#include "Globals.h"
#include <iostream>

LightingManager::LightingManager() {
    // // Par défaut, on peut initialiser une lumière blanche
    // lights.push_back({Vec3(0, 2, -3), Vec3(1,1,1), 1.0f});
}

void LightingManager::addLight(const Light & light) {
    m_lights.push_back(light);
}

void LightingManager::clearLights() {
    m_lights.clear();
}

void LightingManager::setupLightingOnScene() {
    // Paramètres sur la lumière diffuse et ambiante
    m_lightingSettings.m_diffuseIntensity = 0.2f;
    m_lightingSettings.m_ambientColor = Vec3(1.f, 1.f, 1.f);
    m_lightingSettings.m_ambientStrength = 0.4f;

    // Paramètres sur la lumière spéculaire
    m_lightingSettings.m_specularStrength = 0.2f;
    m_lightingSettings.m_shininess = 64.f;

    // Paramètres sur le fog
    m_lightingSettings.m_fogColor = Color(8, 159, 255, 255);
    m_lightingSettings.m_fogStart = 15.0f;
    m_lightingSettings.m_fogEnd = 30.0f;
    m_lightingSettings.m_fogDensity = 0.025f;
    m_lightingSettings.m_fogType = 0; // 0: aucun, 1: linéaire, 2: exp, 3: exp²
}

void LightingManager::applyLightning(Shader & shader, const Vec3 & viewPos) const {
    int activeCount = 0;
    for (const auto & light : m_lights) {
        if (light.isActive())
            activeCount++;
    }
    if (activeCount > MAX_LIGHTS) {
        std::cerr << "Warning: More active lights than MAX_LIGHTS (" << MAX_LIGHTS << "). Clamping to " << MAX_LIGHTS << ".\n";
        activeCount = MAX_LIGHTS;
    }

    shader.set("numLights", activeCount);

    int sentIndex = 0;
    for (int i = 0; i < (int)m_lights.size(); i++) {
        if (!m_lights[i].isActive())
            continue;
        std::string idx = std::to_string(sentIndex);
        shader.set("lights[" + idx + "].type", (int)m_lights[i].getType());
        shader.set("lights[" + idx + "].position", m_lights[i].getPosition());
        shader.set("lights[" + idx + "].direction", m_lights[i].getDirection().normalized());
        shader.set("lights[" + idx + "].color", Vec3(m_lights[i].getColor().r, m_lights[i].getColor().g, m_lights[i].getColor().b));
        shader.set("lights[" + idx + "].intensity", m_lights[i].getIntensity());
        shader.set("lights[" + idx + "].constant", m_lights[i].getConstant());
        shader.set("lights[" + idx + "].linear", m_lights[i].getLinear());
        shader.set("lights[" + idx + "].quadratic", m_lights[i].getQuadratic());
        sentIndex++;
    }

    // Paramètres globaux
    shader.set("ambientColor", m_lightingSettings.m_ambientColor);
    shader.set("ambientStrength", m_lightingSettings.m_ambientStrength);

    shader.set("diffuseColor", m_lightingSettings.m_diffuseColor);
    shader.set("diffuseIntensity", m_lightingSettings.m_diffuseIntensity);

    shader.set("specularColor", m_lightingSettings.m_specularColor);
    shader.set("specularStrength", m_lightingSettings.m_specularStrength);
    shader.set("shininess", m_lightingSettings.m_shininess);

    // Paramètres sur le fog
    shader.setVec3("fogColor", Vec3(m_lightingSettings.m_fogColor.r,
                                    m_lightingSettings.m_fogColor.g,
                                    m_lightingSettings.m_fogColor.b));
    shader.setFloat("fogStart", m_lightingSettings.m_fogStart);
    shader.setFloat("fogEnd", m_lightingSettings.m_fogEnd);
    shader.setFloat("fogDensity", m_lightingSettings.m_fogDensity);
    shader.setInt("fogType", m_lightingSettings.m_fogType);

    shader.set("viewPos", viewPos);
}

void LightingManager::applyPosLights(Shader & shader, const Vec3 & lightColor) const {
    shader.use();
    shader.set("color", lightColor);
}

const Light & LightingManager::getFirstDirectional() const {
    for (const Light & light : m_lights) {
        if (light.getType() == LightType::LIGHT_DIRECTIONAL) {
            return light;
        }
    }
    static Light dummyLight;
    dummyLight.setType(LightType::LIGHT_ERROR);
    return dummyLight;
}

Light * LightingManager::getFirstDirectional() {
    for (Light & light : m_lights) {
        if (light.getType() == LightType::LIGHT_DIRECTIONAL) {
            return &light;
        }
    }
    static Light dummyLight;
    dummyLight.setType(LightType::LIGHT_ERROR);
    return &dummyLight;
}

const std::vector<Light> LightingManager::getPonctualLights() const {
    std::vector<Light> ponctualLights;
    for (const Light & light : m_lights) {
        if (light.getType() == LightType::LIGHT_POINT && light.isActive()) {
            ponctualLights.emplace_back(light);
        }
    }
    return ponctualLights;
}

std::vector<Light> * LightingManager::getPonctualLightsPtr() {
    std::vector<Light> * ponctualLights = new std::vector<Light>();
    for (Light & light : m_lights) {
        if (light.getType() == LightType::LIGHT_POINT) {
            ponctualLights->emplace_back(light);
        }
    }
    return ponctualLights;
}

std::vector<Light *> LightingManager::getPonctualLightsRef() {
    std::vector<Light *> ponctualLights;
    for (Light & light : m_lights) {
        if (light.getType() == LightType::LIGHT_POINT) {
            ponctualLights.push_back(&light);
        }
    }
    return ponctualLights;
}
