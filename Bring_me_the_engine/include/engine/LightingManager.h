#pragma once
#include "engine/Light.h"
#include "engine/LightSettings.h"
#include "math/Vec.h"
#include "rendering/Shader.h"
#include <vector>

/**
 * @class LightingManager
 * @brief Gère toutes les lumières d'une scène et leur application dans les shaders.
 *
 * Permet d'ajouter, supprimer et accéder aux lumières, de configurer les paramètres globaux
 * de l'éclairage, et de mettre à jour les uniforms GLSL pour le rendu.
 */
class LightingManager {
public:
    /// Constructeur par défaut
    LightingManager();

    /// Ajoute une lumière à la scène
    void addLight(const Light & light);

    /// Supprime toutes les lumières de la scène
    void clearLights();

    /**
     * @brief Met à jour les uniforms de lumière dans le shader.
     *
     * Envoie les informations de la lumière principale (directionnelle) et la position de la caméra.
     * @param shader Shader à mettre à jour
     * @param viewPos Position de la caméra
     */
    void applyLightning(Shader & shader, const Vec3 & viewPos) const;

    /// Applique les positions des lumières ponctuelles au shader
    void applyPosLights(Shader & shader, const Vec3 & lightColor) const;

    /// Accès aux paramètres pour pouvoir les modifier
    LightSettings & settings() { return m_lightingSettings; }
    const LightSettings & settings() const { return m_lightingSettings; }

    /// Retourne toutes les lumières
    const std::vector<Light> & getLights() const { return m_lights; }

    /// Retourne la première lumière directionnelle
    const Light & getFirstDirectional() const;
    Light * getFirstDirectional();

    /// Retourne toutes les lumières ponctuelles
    const std::vector<Light> getPonctualLights() const;
    std::vector<Light> * getPonctualLightsPtr();
    std::vector<Light *> getPonctualLightsRef();

    /// Configure l'éclairage de la scène (envoi aux shaders)
    void setupLightingOnScene();

private:
    /// Liste des lumières dans la scène
    /// @note GLSL : MAX_LIGHTS = 8, attention à ne pas dépasser
    std::vector<Light> m_lights;

    /// Paramètres globaux de l'éclairage (intensité ambiante, etc.)
    LightSettings m_lightingSettings;
};
