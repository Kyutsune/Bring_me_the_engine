#include "engine/Scene.h"
#include "engine/SceneLoader/SceneLoader.h"
#include "math/Vec.h"
#include "rendering/GestionTextures/TextureManager.h"
#include "rendering/Shader.h"
#include "system/PathResolver.h"
#include <algorithm>

Scene::Scene() : m_view(Mat4::identity()), m_projection(Mat4::identity()) {
    m_camera = Camera(
        Vec3(-5.68114, 0.024997, -0.912596), // position à l'initialisation
        Vec3(0.025365, 0.0263991, -3.00793), // le point ciblé
        Vec3(0, 1, 0),                       // up
        45.0f,                               // FOV
        1600.0f / 800.0f,                    // aspect ratio
        0.1f, 100.0f);                       // near et far planes

    m_view = m_camera.getViewMatrix();
    m_projection = m_camera.getProjectionMatrix();

    SceneLoader::loadScene(1, m_entities, m_lightEntities, m_skybox, m_lightingManager);
}

// Initialisation de base dans une scène
Scene::Scene(const int & sceneIndex) : m_view(Mat4::identity()), m_projection(Mat4::identity()) {
    m_camera = Camera(
        Vec3(-7, 3, 1), // position à l'initialisation
        Vec3(0., 0., -3.),   // le point ciblé
        Vec3(0, 1, 0),       // up
        45.0f,               // FOV
        1600.0f / 800.0f,    // aspect ratio
        0.1f, 1000.0f);      // near et far planes

    m_view = m_camera.getViewMatrix();
    m_projection = m_camera.getProjectionMatrix();
    SceneLoader::loadScene(sceneIndex, m_entities, m_lightEntities, m_skybox, m_lightingManager);
}

void Scene::update() {
    // Mise à jour des matrices de vue et de projection
    m_view = m_camera.getViewMatrix();
    m_projection = m_camera.getProjectionMatrix();

    // Mise à jour du frustum pour le culling
    m_frustum.update(m_view * m_projection);

    float angle = glfwGetTime();
    if (!m_entities.empty() && m_entities[0]->getName() == "Cube_qui_tourne") {
        Mat4 rotation = Mat4::rotateY(angle) * Mat4::rotateZ(angle);
        Mat4 translation = Mat4::Translation({0, 0, -1});

        m_entities[0]->setTransform(translation * rotation);
    }
}

void Scene::removeEntity(const std::shared_ptr<Entity> & entity) {
    // FIXME: mes textures restent en mémoire après la suppression de l'entité dans la scène,
    //  Il faudrait vérifier si pas de soucis lors de la suppression de l'entité qui pourrait
    //  Causer des soucis de performances ou de mémoire. (ou les deux)
    // (La texture doit rester en mémoire pour le moment hein, mais pas l'entité)
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it != m_entities.end()) {
        m_entities.erase(it);
    }
}
