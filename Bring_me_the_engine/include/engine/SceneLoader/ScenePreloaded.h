#pragma once

#include "engine/Entity.h"
#include "engine/LightingManager.h"
#include "rendering/Skybox.h"

namespace scenePreloaded {

    /**
     * @brief Charge la scène préchargée 1
     * 
     * @param entities Référence vers le vecteur des entités de la scène
     * @param lightEntities Référence vers le vecteur des entités représentant des lumières
     * @param skybox Référence vers le pointeur unique de la skybox
     * @param lightingManager Référence vers le gestionnaire de lumières
     */
    void loadScene1(std::vector<std::shared_ptr<Entity>> & entities,
                    std::vector<std::shared_ptr<Entity>> & lightEntities,
                    std::unique_ptr<Skybox> & skybox,
                    LightingManager & lightingManager);

    /**
     * @brief Charge la scène préchargée 2
     * 
     * @param entities Référence vers le vecteur des entités de la scène
     * @param lightEntities Référence vers le vecteur des entités représentant des lumières
     * @param skybox Référence vers le pointeur unique de la skybox
     * @param lightingManager Référence vers le gestionnaire de lumières
     */
    void loadScene2(std::vector<std::shared_ptr<Entity>> & entities,
                    std::vector<std::shared_ptr<Entity>> & lightEntities,
                    std::unique_ptr<Skybox> & skybox,
                    LightingManager & lightingManager);

    /**
     * @brief Charge une scène depuis un fichier JSON
     * 
     * @param filePath Chemin vers le fichier JSON
     * @param entities Référence vers le vecteur des entités de la scène
     * @param lightEntities Référence vers le vecteur des entités représentant des lumières
     * @param skybox Référence vers le pointeur unique de la skybox
     * @param lightingManager Référence vers le gestionnaire de lumières
     */
    void loadSceneFromJson(const std::string & filePath,
                           std::vector<std::shared_ptr<Entity>> & entities,
                           std::vector<std::shared_ptr<Entity>> & lightEntities,
                           std::unique_ptr<Skybox> & skybox,
                           LightingManager & lightingManager);
}
