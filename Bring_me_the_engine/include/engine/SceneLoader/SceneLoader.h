#pragma once
#include "engine/Entity.h"
#include "engine/LightingManager.h"
#include "rendering/Skybox.h"
#include <memory>
#include <string>
#include <vector>

namespace SceneLoader {

    /**
     * @brief Charge une scène en fonction de son index prédéfini
     * 
     * @param sceneIndex Index de la scène à charger
     * @param m_entities Référence vers le vecteur des entités de la scène
     * @param m_lightEntities Référence vers le vecteur des entités représentant des lumières
     * @param m_skybox Référence vers le pointeur unique de la skybox
     * @param lightingManager Référence vers le gestionnaire de lumières
     */
    void loadScene(const int & sceneIndex,
                   std::vector<std::shared_ptr<Entity>> & m_entities,
                   std::vector<std::shared_ptr<Entity>> & m_lightEntities,
                   std::unique_ptr<Skybox> & m_skybox,
                   LightingManager & lightingManager);

    /**
     * @brief Charge une scène depuis son nom ou identifiant
     * 
     * @param sceneName Nom de la scène à charger
     */
    void loadScene(const std::string & sceneName);

}
