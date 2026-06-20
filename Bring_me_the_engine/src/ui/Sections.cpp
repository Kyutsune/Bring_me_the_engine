#include "ui/Sections.h"
#include "ui/SectionsUtilitary.h"
#include "engine/Scene.h"
#include "system/UtilsFile.h"
#include "Globals.h"

#include "imgui.h"
#include <unordered_map>

namespace Sections {

    void sensitivitySection() {
        if (ImGui::CollapsingHeader("Sensibilité")) {
            ImGui::SeparatorText("Sensibilité de la souris");
            if (ImGui::SliderFloat("Sensibilité rotation##SliderRot", &g_sensibility_rot, 0.001f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)){
                UtilsFile::saveSettingsSensRot(g_settingsFilePath);
            }
            if (ImGui::InputFloat("Sensibilité rotation##InputRot", &g_sensibility_rot, 0.001f, 1.f, "%.3f")) {
                UtilsFile::saveSettingsSensRot(g_settingsFilePath);
            }


            ImGui::SeparatorText("Sensibilité du clavier");
            if (ImGui::SliderFloat("Sensibilité déplacement##Sliderdepl", &g_sensibility_depl, 0.01f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
                UtilsFile::saveSettingsSensRot(g_settingsFilePath);
            }
            if (ImGui::InputFloat("Sensibilité déplacement##Inputdepl", &g_sensibility_depl, 0.01f, 1.f, "%.2f")) {
                UtilsFile::saveSettingsSensRot(g_settingsFilePath);
            }
            ImGui::Separator();


            if (ImGui::Button("Réinitialisation rotation")) {
                g_sensibility_rot = 0.01f;
				UtilsFile::saveSettingsSensRot(g_settingsFilePath);
            }
            ImGui::SameLine();
            if (ImGui::Button("Réinitialisation déplacement")) {
                g_sensibility_depl = 0.10f;
				UtilsFile::saveSettingsSensRot(g_settingsFilePath);
            }
        }
    }

    void fogSection(Scene * scene) {
        if (ImGui::CollapsingHeader("Fog")) {
            ImGui::SeparatorText("Type de fog actif");
            static const char * s_items[] = {"Aucun", "Linéaire", "Exponentiel", "Exponentiel²"};
            static int s_currentFogType = scene->getFogType();
            if (ImGui::Combo("Type de Fog", &s_currentFogType, s_items, IM_ARRAYSIZE(s_items))) {
                scene->setFogType(s_currentFogType);
            }

            ImGui::SeparatorText("Valeurs du Fog");
            ImGui::SliderFloat("Début", scene->getFogStart(), 0.0f, *scene->getFogEnd(), "%.1f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Fin", scene->getFogEnd(), *scene->getFogStart(), 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
            float fogDensity = *scene->getFogDensity();
            if (ImGui::SliderFloat("Densité##slider", &fogDensity, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp) ||
                ImGui::InputFloat("Densité##input", &fogDensity, 0.01f, 0.1f, "%.3f")) {
                scene->setFogDensity(fogDensity);
            }

            ImGui::SeparatorText("Couleur du Fog");
            Color & fogColor = scene->getFogColor();
            float colorTmp[3] = {
                fogColor.r,
                fogColor.g,
                fogColor.b};

            if (ImGui::ColorEdit3("Couleur", colorTmp)) {
                fogColor.r = colorTmp[0];
                fogColor.g = colorTmp[1];
                fogColor.b = colorTmp[2];
            }
        }
    }

    void quitButton(GLFWwindow * window) {
        if (ImGui::Button("Quitter")) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    void textureSection(Scene* scene) {
        if (ImGui::CollapsingHeader("Textures")) {
            for (auto& entityPtr : scene->getEntities()) {
                // On utilise le pointeur de l'entité comme ID parent
                ImGui::PushID(entityPtr.get());

                ImGui::SeparatorText(entityPtr->getName().c_str());

                int subIdx = 0;
                for (auto& sub : entityPtr->getSubMeshes()) {
                    ImGui::PushID(subIdx++); 

                    ImGui::Text("Partie %d", subIdx);

                    Material& mat = const_cast<Material&>(sub.material);

                    if (mat.m_diffuseTexture)
                        ImGui::Checkbox("Utiliser texture diffuse", &mat.m_useDiffuse);
                    if (mat.m_normalMap)
                        ImGui::Checkbox("Utiliser normal map", &mat.m_useNormal);
                    if (mat.m_specularMap)
                        ImGui::Checkbox("Utiliser specular map", &mat.m_useSpecular);

                    ImGui::PopID();
                }

                ImGui::PopID();
            }
        }
    }

    void lightSection(Scene * scene) {
        if (g_forceOpenLightHeader) {
            ImGui::SetNextItemOpen(true, ImGuiCond_Always);
        }

        if (ImGui::CollapsingHeader("Lumières")) {
            // Identification de la lumière actuellement sélectionnée
            std::string expandedName = "";
            for (auto const & [name, expanded] : g_lightExpanded) {
                if (expanded) {
                    expandedName = name;
                    break;
                }
            }

            Light * dirLightPtr = scene->getLightingManager().getFirstDirectional();
            bool isDirExpanded = false;

            // On vérifie si l'entité sélectionnée correspond à la directionnelle
            for (const auto & entity : scene->getLightEntitiesRef()) {
                if (entity->getName() == expandedName) {
                    if ((entity->getPosition() - dirLightPtr->getPosition()).length() < 0.001f) {
                        isDirExpanded = true;
                    }
                    break;
                }
            }

            // Section Directionnelle
            if (g_forceOpenLightHeader && isDirExpanded)
                ImGui::SetNextItemOpen(true, ImGuiCond_Always);
            if (ImGui::TreeNode("Lumière directionnelle")) {
                directionnalLightSection(scene, isDirExpanded ? expandedName : "");
                ImGui::TreePop();
            }

            // Section Ponctuelle
            if (g_forceOpenLightHeader && !isDirExpanded && !expandedName.empty())
                ImGui::SetNextItemOpen(true, ImGuiCond_Always);
            if (ImGui::TreeNode("Lumières ponctuelles")) {
                ponctualLightSection(scene, dirLightPtr);
                ImGui::TreePop();
            }
        }

        if (g_forceOpenLightHeader) {
            g_forceOpenLightHeader = false;
        }
    }

    void directionnalLightSection(Scene * scene, const std::string & name) {
        ImGui::SeparatorText("Paramètres de la lumière directionnelle");
        LightingManager & lightingManager = scene->getLightingManager();
        Light * light = lightingManager.getFirstDirectional();

        if (!name.empty()) {
            if (ImGui::Selectable(name.c_str(), g_lightExpanded[name])) {
                g_lightExpanded.clear();
                g_lightExpanded[name] = true;
            }
        }

        bool isActive = light->isActive();
        if (ImGui::Checkbox("Activer", &isActive)) {
            light->setActive(isActive);
        }
        float intensity = light->getIntensity() * 10.0f;
        if (ImGui::SliderFloat("Intensité", &intensity, 0.0f, 400.0f, "%.0f%%", ImGuiSliderFlags_AlwaysClamp)) {
            light->setIntensity(intensity / 10.0f);
        }
        // TODO: Modifier la direction
    }

    void ponctualLightSection(Scene * scene, Light * dirLightPtr) {
        ImGui::SeparatorText("Paramètres des lumières ponctuelles");

        const std::vector<std::shared_ptr<Entity>> & lightEntities = scene->getLightEntitiesRef();
        std::vector<Light *> lights = scene->getLightingManager().getPonctualLightsRef();

        int punctualIdx = 0;
        for (const auto & entity : lightEntities) {
            // On saute l'entité qui correspond à la lumière directionnelle
            if ((entity->getPosition() - dirLightPtr->getPosition()).length() < 0.001f) {
                continue;
            }

            if (punctualIdx >= (int)lights.size())
                break;

            Light * light = lights[punctualIdx];
            std::string name = entity->getName();
            ImGui::PushID(light);

            bool isExpanded = g_lightExpanded[name];
            if (ImGui::Selectable(name.c_str(), isExpanded)) {
                g_lightExpanded.clear();
                g_lightExpanded[name] = !isExpanded;
            }

            if (g_forceOpenLightHeader && g_lightExpanded[name]) {
                ImGui::SetScrollHereY(0.5f); 
            }

            if (g_lightExpanded[name]) {
                ImGui::Indent();
                ImGui::SeparatorText(("Paramètres : " + name).c_str());

                bool isActive = light->isActive();
                if (ImGui::Checkbox("Activer", &isActive)) {
                    light->setActive(isActive);
                }

                float intensity = light->getIntensity() * 10.f;
                if (ImGui::SliderFloat("Intensité", &intensity, 0.0f, 200.0f, "%.0f%%", ImGuiSliderFlags_AlwaysClamp)) {
                    light->setIntensity(intensity / 10.f);
                }

                ImGui::SeparatorText("Position de la lumière");
                Vec3 position = light->getPosition();
                if (ImGui::DragFloat3("Position", &position.x, 0.01f, -100.0f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
                    light->setPosition(position);
                    entity->setPosition(position);
                }

                SectionsUtilitary::renderPositionEditor(name, position, [&](const Vec3 & newPos) {
                    light->setPosition(newPos);
                    entity->setPosition(newPos);
                });

                ImGui::SeparatorText("Couleur de la lumière");
                Color & lightColor = light->getColor();
                float colorTmp[3] = {lightColor.r, lightColor.g, lightColor.b};
                if (ImGui::ColorEdit3("Couleur", colorTmp)) {
                    lightColor.r = colorTmp[0];
                    lightColor.g = colorTmp[1];
                    lightColor.b = colorTmp[2];
                }

                ImGui::Unindent();
                ImGui::Separator();
            }

            ImGui::PopID();
            punctualIdx++;
        }
    }

    bool SceneSection(Scene * scene) {
        if (ImGui::CollapsingHeader("Scène")) {
            static int currentScene = g_sceneIndex - 1;
            static int selectedScene = g_sceneIndex - 1;

            ImGui::SeparatorText("Séléction de la scène courante");
            if (ImGui::Combo("Scene", &selectedScene, "Scene 1\0Scene 2\0")) {
                if (selectedScene != currentScene) {
                    currentScene = selectedScene;
                    g_scenePtr.reset();
                    std::cout << "Changement de scène: " << currentScene + 1 << std::endl;
                    g_scenePtr = std::make_unique<Scene>(currentScene + 1);
                    g_scene = g_scenePtr.get();
                    return true;
                }
            }
        }
        return false;
    }

    void objectSection(Scene * scene) {
        // Si on clique sur une entité, on force l'ouverture de cette section là
        if (g_forceOpenObjectHeader) {
            ImGui::SetNextItemOpen(true, ImGuiCond_Always);
        }
        std::shared_ptr<Entity> entityToDelete = nullptr;

        if (ImGui::CollapsingHeader("Objets")) {
            for (const std::shared_ptr<Entity> & entity : scene->getEntities()) {
                ImGui::PushID(entity.get());
                const std::string & name = entity->getName();
                if (name.empty())
                    continue;


                bool isExpanded = g_entityExpanded[name];
                if (ImGui::Selectable(name.c_str(), isExpanded)) {
                    g_entityExpanded[name] = !g_entityExpanded[name];
                }

                if (g_forceOpenObjectHeader && isExpanded) {
                    ImGui::SetScrollHereY(0.5f);
                }

                if (g_entityExpanded[name]) {
                    ImGui::Indent();

                    ImGui::SeparatorText("Position");

                    Vec3 position = entity->getTransform().getTranslation();

                    ImGui::Text("(%.2f, %.2f, %.2f)", position.x, position.y, position.z);

                    if (ImGui::DragFloat3("Position", &position.x, 0.01f, -100.0f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
                        entity->setPosition(position);
                    }

                    SectionsUtilitary::renderPositionEditor(name + " Position", position, [&](const Vec3 & newPos) {
                        entity->setPosition(newPos);
                    });

                    ImGui::SeparatorText("Echelle");
                    Vec3 scale = entity->getScale();
                    if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.01f, 100.0f, "%.2f")) {
                        entity->setScale(scale);
                    }

                    ImGui::SeparatorText("Rotation");
                    Vec3 currentEulerDeg = entity->getRotation().toEuler() * (180.0f / M_PI);
                    ImGui::Text("Orientation actuelle : (%.1f°, %.1f°, %.1f°)", currentEulerDeg.x, currentEulerDeg.y, currentEulerDeg.z);

                    static Vec3 deltaRotation = {0, 0, 0};
                    if (ImGui::DragFloat3("", &deltaRotation.x, 0.1f)) {
                        Quat delta = Quat::fromEuler(deltaRotation * (M_PI / 180.0f));
                        entity->setRotation(delta * entity->getRotation());
                        deltaRotation = {0, 0, 0};
                    }

                    if (!entity->hasTextureDiffuse()) {
                        ImGui::SeparatorText("Couleur de base");
                        Vec3 baseColor = entity->getBaseColor();
                        if (ImGui::ColorEdit3("Base Color", &baseColor.x)) {
                            entity->setBaseColor(baseColor);
                        }
                    }

                    if (ImGui::Button("Réinitialiser la rotation")) {
                        entity->setRotation(Quat::identity());
                        deltaRotation = {0, 0, 0};
                    }

                    if (ImGui::Button("Supprimer l'objet")) {
                        entityToDelete = entity;
                    }

                    ImGui::Unindent();
                    ImGui::Separator();
                }

                ImGui::PopID();
            }

            if (g_forceOpenObjectHeader) {
                g_forceOpenObjectHeader = false;
            }

            if (entityToDelete) {
                scene->removeEntity(entityToDelete);
                g_entityExpanded.erase(entityToDelete->getName());
				updatePerformanceStatsOnRemovedEntity(*entityToDelete);
            }
        }
    }

    void changeEntityCreatedSection(Scene * scene) {
        if (ImGui::CollapsingHeader("Objets à créer")) {
            ImGui::SeparatorText("Type d'entité à créer");

            static const char * items[] = {"Cube", "Sphere"};

            int currentItem = -1;
            if (g_typeEntityCreated == TypeEntityCreated::Cube)
                currentItem = 0;
            else if (g_typeEntityCreated == TypeEntityCreated::Sphere)
                currentItem = 1;

            if (ImGui::Combo("Type d'entité", &currentItem, items, IM_ARRAYSIZE(items))) {
                if (currentItem == 0)
                    g_typeEntityCreated = TypeEntityCreated::Cube;
                else if (currentItem == 1)
                    g_typeEntityCreated = TypeEntityCreated::Sphere;
            }
            ImGui::ColorEdit3("Couleur", &g_selectedColor.x);
        }
    }

    void performanceSection() {
        if (ImGui::CollapsingHeader("Performances: CPU/GPU")) {
            ImGui::Text("GPU Frame Time: %.3f ms", g_perfStats.gpuFrameTimeMs);
            ImGui::Text("GPU Avg Time: %.3f ms", g_perfStats.gpuAvgMs);
            ImGui::Text("GPU FPS: %.2f", g_perfStats.gpuFps);
            ImGui::Separator();
            ImGui::Text("CPU Frame Time: %.3f ms", g_perfStats.cpuFrameTimeMs);
            ImGui::Text("CPU FPS: %.2f", g_perfStats.cpuFps);
            ImGui::Separator();
            ImGui::Text("FPS Final: %.2f", g_perfStats.finalFps);
            ImGui::Text("FPS Final (moyenne): %.2f", g_perfStats.avgFinalFps);
        }
        if (ImGui::CollapsingHeader("Performances: Entitées rendues")) {
            ImGui::Text("Entités dessinées: %.0f", g_perfStats.numberEntitiesDrawn);
            ImGui::Text("Triangles rendus: %.0f", g_perfStats.numberTrianglesRendered);
            ImGui::Text("Points rendus: %.0f", g_perfStats.numberPointsRendered);

            ImGui::Separator();

            ImGui::Text("Total entités dans la scène: %.0f", g_perfStats.totalNumberEntitiesInScene);
            ImGui::Text("Total triangles dans la scène: %.0f", g_perfStats.totalNumberTrianglesInScene);
			ImGui::Text("Total points dans la scène: %.0f", g_perfStats.totalNumberPointsInScene);

            ImGui::Separator();
            float percentage = 0.0f;
            if (g_perfStats.totalNumberTrianglesInScene > 0) {
                percentage = (g_perfStats.numberTrianglesRendered / g_perfStats.totalNumberTrianglesInScene) * 100.0f;
            }

            // Affichage avec une barre de progression pour que ce soit plus visuel
            ImGui::Text("%% de triangles rendus:");
            ImGui::SameLine();
            ImGui::Text("%.2f%%", percentage);

            // Une barre de progression est super utile pour voir l'efficacité du culling d'un coup d'oeil
            ImGui::ProgressBar(percentage / 100.0f, ImVec2(-1.0f, 0.0f));
            
        }
    }
}
