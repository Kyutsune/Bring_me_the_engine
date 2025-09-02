#pragma once

#include "rendering/Shader.h"
#include "rendering/Skybox.h"
#include "camera/Camera.h"
#include "camera/Frustum.h"
#include "engine/Entity.h"
#include "engine/LightingManager.h"

#include <iostream>
#include <memory>
#include <vector>

/**
 * @class Scene
 * @brief Représente une scène 3D complète contenant caméra, entités, lumières et skybox.
 * 
 * La classe gère les entités visibles et lumineuses, la caméra et le frustum pour le culling.
 * Elle fournit des méthodes pour accéder aux entités, à la caméra, au fog, et aux paramètres de lumière.
 */
class Scene {
public:
    /// Constructeur par défaut
    Scene();

    /// Constructeur avec un index de scène (utile pour charger différentes configurations)
    Scene(const int & sceneIndex);

    /// Met à jour la scène, typiquement à chaque frame
    void update();

    /// Initialise les objets de la scène
    void initObjects();

    /// Accès à la caméra de la scène
    inline Camera * getCamera() { return &m_camera; }
    inline const Camera & getCamera() const { return m_camera; }

    /// Accès à la matrice de vue
    inline Mat4 & getView() { return m_view; }

    /// Accès au gestionnaire de lumières
    inline LightingManager & getLightingManager() { return m_lightingManager; }
    inline const LightingManager & getLightingManager() const { return m_lightingManager; }

    /// Accès à la skybox
    inline const Skybox * getSkybox() const { return m_skybox.get(); }

    /// Accès aux entités de la scène
    inline std::vector<std::shared_ptr<Entity>> & getEntities() { return m_entities; }
    inline const std::vector<std::shared_ptr<Entity>> & getEntities() const { return m_entities; }

    /// Accès aux entités représentant des sources de lumière
    inline const std::vector<std::shared_ptr<Entity>> & getLightEntities() const { return m_lightEntities; }

    /// Accès au frustum pour le culling
    inline const Frustum & getFrustum() const { return m_frustum; }

    /// Paramètres de fog
    inline const int getFogType() const { return m_lightingManager.settings().m_fogType; }
    inline void setFogType(int type) { m_lightingManager.settings().m_fogType = type; }
    inline float * getFogStart() { return &m_lightingManager.settings().m_fogStart; }
    inline float * getFogEnd() { return &m_lightingManager.settings().m_fogEnd; }
    inline float * getFogDensity() { return &m_lightingManager.settings().m_fogDensity; }
    inline void setFogDensity(float density) { m_lightingManager.settings().m_fogDensity = density; }
    inline Color & getFogColor() { return m_lightingManager.settings().m_fogColor; }

    /// Supprime une entité de la scène
    void removeEntity(const std::shared_ptr<Entity> & entity);

private:
    Camera m_camera;                                  ///< Caméra de la scène
    Mat4 m_view;                                      ///< Matrice de vue
    Mat4 m_projection;                                ///< Matrice de projection

    std::vector<std::shared_ptr<Entity>> m_entities;  ///< Entités visibles de la scène

    LightingManager m_lightingManager;               ///< Gestionnaire de lumières
    std::vector<std::shared_ptr<Entity>> m_lightEntities; ///< Entités représentant les lumières

    std::unique_ptr<Skybox> m_skybox;                ///< Skybox de la scène

    Frustum m_frustum;                               ///< Frustum de la caméra pour le culling
};
