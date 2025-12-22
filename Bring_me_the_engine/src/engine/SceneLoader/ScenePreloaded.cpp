#include "engine/SceneLoader/ScenePreloaded.h"
#include "engine/Mesh.h"
#include "geometry/Cube.h"
#include "geometry/Floor.h"
#include "geometry/Sphere.h"
#include "rendering/GestionTextures/TextureManager.h"
#include "system/PathResolver.h"
#include "engine/MeshLoader/MeshLoaderObj.h"
#include "Globals.h"
#include <fstream>

#include "../external/json/json.hpp"
using json = nlohmann::json;

namespace scenePreloaded {
    void loadScene1(std::vector<std::shared_ptr<Entity>> & entities,
                    std::vector<std::shared_ptr<Entity>> & lightEntities,
                    std::unique_ptr<Skybox> & skybox,
                    LightingManager & lightingManager) {

        /// Gestion des lumières dans la scène
        lightingManager.setupLightingOnScene();
        lightingManager.addLight({true,
                                  LightType::LIGHT_POINT,
                                  Vec3(3, 1, -1),
                                  Vec3(0, -1, 0),
                                  Color(1.f, 1.f, 1.f, 1.f),
                                  8.f,
                                  1.0f, 0.7f, 1.8f});

        lightingManager.addLight({false,
                                  LightType::LIGHT_DIRECTIONAL,
                                  Vec3(0, 10, 0),
                                  Vec3(1, -1, 0),
                                  Color(255, 255, 255),
                                  10.f,
                                  0.5f, 0.7f, 1.8f});

        lightingManager.addLight({true,
                                  LightType::LIGHT_POINT,
                                  Vec3(-1, 1, -3),
                                  Vec3(0, -1, 0),
                                  Color(255, 255, 255),
                                  7.f,
                                  1.0f, 0.7f, 1.8f});

        // Entité qui sera utilisée pour représenter les lumières ponctuelles dans la scène
        std::shared_ptr<Mesh> lightMesh = createSphere<std::shared_ptr<Mesh>>(0.5f, 36, 18, Color::white());
        for (const Light & light : lightingManager.getLights()) {
            Mat4 lightTransform = Mat4::Scale(Vec3(0.1f, 0.1f, 0.1f)) * Mat4::Translation(light.getPosition());
            std::shared_ptr<Entity> lightEntity = std::make_shared<Entity>(lightTransform, lightMesh);
            lightEntities.emplace_back(lightEntity);
        }

        /// Cubemap pour le skybox
        std::vector<std::string> faces = {
            PathResolver::getResourcePath("assets/cubemap/Nuit_bleue/right.jpg"),
            PathResolver::getResourcePath("assets/cubemap/Nuit_bleue/left.jpg"),
            PathResolver::getResourcePath("assets/cubemap/Nuit_bleue/bottom.jpg"),
            PathResolver::getResourcePath("assets/cubemap/Nuit_bleue/top.jpg"),
            PathResolver::getResourcePath("assets/cubemap/Nuit_bleue/front.jpg"),
            PathResolver::getResourcePath("assets/cubemap/Nuit_bleue/back.jpg")
        };

        skybox = std::make_unique<Skybox>(faces);

        /// Gestion des objets dans la scène
        // Cube qui tourne
        std::shared_ptr<Mesh> cubeMesh = createCube<std::shared_ptr<Mesh>>(Color::cyan());

        std::shared_ptr<Texture> cuivre_diffuse = TextureManager::load(PathResolver::getResourcePath("assets/materiaux/cuivre_diffus.jpg"));
        std::shared_ptr<Texture> cuivre_normal = TextureManager::load(PathResolver::getResourcePath("assets/materiaux/cuivre_normal.jpg"));
        std::shared_ptr<Texture> cuivre_specular = TextureManager::load(PathResolver::getResourcePath("assets/materiaux/cuivre_specular.jpg"));

        std::shared_ptr<Material> cuivreMaterial = std::make_shared<Material>(cuivre_diffuse, cuivre_normal, cuivre_specular);

        std::shared_ptr<Entity> cube_qui_tourne = std::make_shared<Entity>(
            Mat4::identity(), cubeMesh, cuivreMaterial, "Cube_qui_tourne");

        cube_qui_tourne->getBoundingBox().setupBBoxBuffers();

        entities.emplace_back(cube_qui_tourne);

        // Cube tout bleu
        std::shared_ptr<Mesh> cubeMesh2 = createCube<std::shared_ptr<Mesh>>(Color::rose());
        Mat4 t2 = Mat4::Translation(Vec3(1, 0, -5));
        std::shared_ptr<Entity> Cube_tout_bleu = std::make_shared<Entity>(t2, cubeMesh2,
                                                                          "",
                                                                          "",
                                                                          "",
                                                                          "Cube_tout_bleu");
        Cube_tout_bleu->getBoundingBox().setupBBoxBuffers();
        entities.emplace_back(Cube_tout_bleu);

        // Cube plein de texture
        std::shared_ptr<Mesh> cubeMesh3 = createCube<std::shared_ptr<Mesh>>();
        Mat4 t4 = Mat4::Translation(Vec3(3, 0, -3));

        std::shared_ptr<Texture> bois_diffuse = TextureManager::load(PathResolver::getResourcePath("assets/materiaux/bois.jpg"));
        std::shared_ptr<Material> boisMaterial = std::make_shared<Material>(bois_diffuse, nullptr, nullptr);

        std::shared_ptr<Entity> Cube_plein_de_texture = std::make_shared<Entity>(
            t4, cubeMesh3, boisMaterial, "Cube_plein_de_texture");

        Cube_plein_de_texture->getBoundingBox().setupBBoxBuffers();
        entities.emplace_back(Cube_plein_de_texture);

        // Sol béton
        std::shared_ptr<Mesh> floorMesh = createFloor<std::shared_ptr<Mesh>>(25.f, -1.f);

        std::shared_ptr<Texture> sol_diffuse = TextureManager::load(PathResolver::getResourcePath("assets/sol/brique_recyclee/brique_recyclee_diffuse.jpg"));
        std::shared_ptr<Texture> sol_normal = TextureManager::load(PathResolver::getResourcePath("assets/sol/brique_recyclee/brique_recyclee_normal.jpg"));
        std::shared_ptr<Material> solMaterial = std::make_shared<Material>(sol_diffuse, sol_normal, nullptr);

        std::shared_ptr<Entity> sol_beton = std::make_shared<Entity>(
            Mat4::identity(), floorMesh, solMaterial, "Sol_beton");

        sol_beton->getBoundingBox().setupBBoxBuffers();
        entities.emplace_back(sol_beton);

        // Sphère texturée
        std::shared_ptr<Mesh> sphereMesh = createSphere<std::shared_ptr<Mesh>>(0.5f, 36, 18, Color::red());
        Mat4 t5 = Mat4::Translation(Vec3(-2, 0, -5));

        std::shared_ptr<Texture> sphere_diffuse = TextureManager::load(PathResolver::getResourcePath("assets/materiaux/petit_caillou/petit_caillou_diffus.jpg"));
        std::shared_ptr<Texture> sphere_normal = TextureManager::load(PathResolver::getResourcePath("assets/materiaux/petit_caillou/petit_caillou_normal.jpg"));
        std::shared_ptr<Texture> sphere_specular = TextureManager::load(PathResolver::getResourcePath("assets/sol/sol_cobble/sol_cobble_specular.jpg"));
        std::shared_ptr<Material> sphereMaterial = std::make_shared<Material>(sphere_diffuse, sphere_normal, sphere_specular);

        std::shared_ptr<Entity> sphere = std::make_shared<Entity>(
            t5, sphereMesh, sphereMaterial, "Sphere_toute_texturee");

        sphere->getBoundingBox().setupBBoxBuffers();
        entities.emplace_back(sphere);
    }

    void loadScene2(std::vector<std::shared_ptr<Entity>> & entities,
                    std::vector<std::shared_ptr<Entity>> & lightEntities,
                    std::unique_ptr<Skybox> & skybox,
                    LightingManager & lightingManager) {

        lightingManager.setupLightingOnScene();
        lightingManager.addLight({true,
                                  LightType::LIGHT_POINT,
                                  Vec3(3, 1, -1),
                                  Vec3(0, -1, 0),
                                  Color(255, 255, 255),
                                  10.f,
                                  1.0f, 0.7f, 1.8f});

        lightingManager.addLight({false,
                                  LightType::LIGHT_DIRECTIONAL,
                                  Vec3(0, 10, 0),
                                  Vec3(1, -1, 0),
                                  Color(255, 255, 255),
                                  10.f,
                                  0.5f, 0.7f, 1.8f});

        lightingManager.addLight({true,
                                  LightType::LIGHT_POINT,
                                  Vec3(-1, 1, -3),
                                  Vec3(0, -1, 0),
                                  Color(255, 255, 255),
                                  10.f,
                                  1.0f, 0.7f, 1.8f});

        std::shared_ptr<Mesh> lightMesh = createSphere<std::shared_ptr<Mesh>>(0.5f, 36, 18, Color::white());
        for (const Light & light : lightingManager.getLights()) {
            Mat4 lightTransform = Mat4::Scale(Vec3(0.1f, 0.1f, 0.1f)) * Mat4::Translation(light.getPosition());
            std::shared_ptr<Entity> lightEntity = std::make_shared<Entity>(lightTransform, lightMesh);
            lightEntities.emplace_back(lightEntity);
        }

        std::shared_ptr<Mesh> floorMesh = createFloor<std::shared_ptr<Mesh>>(25.f, -1.f);
        // auto sol_beton = std::make_shared<Entity>(t3, floorMesh, "../assets/sol/sol_cobble/sol_cobble.jpg", "../assets/sol/sol_cobble/sol_cobble_normal.jpg", "../assets/sol/sol_cobble/sol_cobble_specular.jpg");
        std::shared_ptr<Entity> sol_beton = std::make_shared<Entity>(Mat4::identity(), floorMesh,
                                                                     PathResolver::getResourcePath("assets/sol/brique_recyclee/brique_recyclee_diffuse.jpg"),
                                                                     PathResolver::getResourcePath("assets/sol/brique_recyclee/brique_recyclee_normal.jpg"),
                                                                     "",
                                                                     "Sol_beton");
        sol_beton->getBoundingBox().setupBBoxBuffers();
        entities.emplace_back(sol_beton);
    }

    void loadSceneFromJson(const std::string & filePath,
                           std::vector<std::shared_ptr<Entity>> & entities,
                           std::vector<std::shared_ptr<Entity>> & lightEntities,
                           std::unique_ptr<Skybox> & skybox,
                           LightingManager & lightingManager) {

        std::ifstream in(PathResolver::getResourcePath(filePath));
        if (!in.is_open()) {
            std::cerr << "❌ Impossible d'ouvrir le fichier JSON : " << filePath << std::endl;
            return;
        }

        json sceneJson;
        try {
            in >> sceneJson;
        } catch (const std::exception & e) {
            std::cerr << "❌ Erreur lors du parsing JSON : " << e.what() << std::endl;
            return;
        }

        // SKYBOX
        if (sceneJson.contains("skybox") && sceneJson["skybox"].contains("faces")) {
            std::vector<std::string> faces;
            for (const auto & face : sceneJson["skybox"]["faces"]) {
                faces.push_back(PathResolver::getResourcePath(face.get<std::string>()));
            }
            skybox = std::make_unique<Skybox>(faces);
        }

        // LIGHTS
        lightingManager.setupLightingOnScene();
        if (sceneJson.contains("lights") && sceneJson["lights"].is_array()) {
            for (const auto & lightJson : sceneJson["lights"]) {
                bool enabled = lightJson.value("enabled", true);
                std::string typeStr = lightJson.value("type", "directional");
                LightType type = typeStr == "point" ? LightType::LIGHT_POINT : LightType::LIGHT_DIRECTIONAL;

                Vec3 position = Vec3(0.0f), direction = Vec3(0.0f, -1.0f, 0.0f);
                if (lightJson.contains("position") && lightJson["position"].is_array() && lightJson["position"].size() == 3) {
                    position = Vec3(lightJson["position"][0], lightJson["position"][1], lightJson["position"][2]);
                }
                if (lightJson.contains("direction") && lightJson["direction"].is_array() && lightJson["direction"].size() == 3) {
                    direction = Vec3(lightJson["direction"][0], lightJson["direction"][1], lightJson["direction"][2]);
                }

                Color color = Color::white();
                if (lightJson.contains("color") && lightJson["color"].is_array() && lightJson["color"].size() == 4) {
                    color = Color(
                        static_cast<float>(lightJson["color"][0]),
                        static_cast<float>(lightJson["color"][1]),
                        static_cast<float>(lightJson["color"][2]),
                        static_cast<float>(lightJson["color"][3]));
                }

                float intensity = lightJson.value("intensity", 1.0f);

                float att_constant = 1.0f, att_linear = 0.0f, att_quad = 0.0f;
                if (lightJson.contains("attenuation") && lightJson["attenuation"].is_array() && lightJson["attenuation"].size() == 3) {
                    att_constant = lightJson["attenuation"][0].get<float>();
                    att_linear = lightJson["attenuation"][1].get<float>();
                    att_quad = lightJson["attenuation"][2].get<float>();
                }

                Light light(enabled, type, position, direction, color, intensity, att_constant, att_linear, att_quad);
                lightingManager.addLight(light);
            }
        }

        // VISUAL LIGHT ENTITIES
        std::shared_ptr<Mesh> lightMesh = createSphere<std::shared_ptr<Mesh>>(g_lightSize, 36, 18, Color::white());
        for (const Light & light : lightingManager.getLights()) {
            Mat4 lightTransform = Mat4::Scale(Vec3(0.1f, 0.1f, 0.1f)) * Mat4::Translation(light.getPosition());
            std::shared_ptr<Entity> lightEntity = std::make_shared<Entity>(lightTransform, lightMesh);
            //TODO: Trouver un moyen plus malin de nommage, car lightEntities comprend aussi les directionnelles
            lightEntity->setName("Light_" + std::to_string(lightEntities.size()));
            lightEntities.emplace_back(lightEntity);
        }

        // ENTITIES
        if (sceneJson.contains("entities") && sceneJson["entities"].is_array()) {
            for (const auto & e : sceneJson["entities"]) {
                std::string type = e.value("mesh", "");
                std::shared_ptr<Mesh> mesh;

                if (type == "cube") {
                    if (e.contains("color") && e["color"].is_array() && e["color"].size() == 3) {
                        mesh = createCube<std::shared_ptr<Mesh>>(Color(
                            static_cast<float>(e["color"][0]),
                            static_cast<float>(e["color"][1]),
                            static_cast<float>(e["color"][2])));
                    } else {
                        mesh = createCube<std::shared_ptr<Mesh>>();
                    }
                } else if (type == "sphere") {
                    float radius = e.value("radius", 0.5f);
                    mesh = createSphere<std::shared_ptr<Mesh>>(radius, 36, 18, Color::white());
                } else if (type == "floor") {
                    mesh = createFloor<std::shared_ptr<Mesh>>(e.value("size", 25.0f), e.value("y", -1.0f));
                } else {
                    if (type.size() >= 4 && type.rfind(".obj") == (type.size() - 4)) {
                        std::string objPath = PathResolver::getResourcePath(type);
                        mesh = MeshLoaderOBJ::loadFromFile(objPath);
                    } else {
                        std::cerr << "⚠️ Type de mesh non reconnu : " << type << "Dans ScenePreloaded" << std::endl;
                        continue;
                    }
                }

                // Transformation
                Vec3 translation(0.f), scale(1.f), rotation(0.f);
                if (e.contains("transform")) {
                    const auto & t = e["transform"];
                    if (t.contains("translate") && t["translate"].is_array() && t["translate"].size() == 3) {
                        translation = Vec3(t["translate"][0], t["translate"][1], t["translate"][2]);
                    }
                    if (t.contains("scale") && t["scale"].is_array() && t["scale"].size() == 3) {
                        scale = Vec3(t["scale"][0], t["scale"][1], t["scale"][2]);
                    }
                    if (t.contains("rotate") && t["rotate"].is_array() && t["rotate"].size() == 3) {
                        rotation = Vec3(t["rotate"][0], t["rotate"][1], t["rotate"][2]);
                    }
                }

                Mat4 transform = Mat4::Scale(scale) * Mat4::rotateXYZ(rotation) * Mat4::Translation(translation);

                // Matériaux
                std::shared_ptr<Material> material = nullptr;
                if (e.contains("material")) {
                    /// TODO: Faire en sorte que mes fichiers puissent charger des couleurs de base aussi
                    std::string diffuse = e["material"].value("diffuse", "");
                    std::string normal = e["material"].value("normal", "");
                    std::string specular = e["material"].value("specular", "");

                    std::shared_ptr<Texture> d = diffuse.empty() ? nullptr : TextureManager::load(PathResolver::getResourcePath(diffuse));
                    std::shared_ptr<Texture> n = normal.empty() ? nullptr : TextureManager::load(PathResolver::getResourcePath(normal));
                    std::shared_ptr<Texture> s = specular.empty() ? nullptr : TextureManager::load(PathResolver::getResourcePath(specular));

                    material = std::make_shared<Material>(d, n, s);
                }

                std::string name = e.value("name", "Unnamed");

                std::shared_ptr<Entity> entity = std::make_shared<Entity>(transform, mesh, material, name);
                entity->getBoundingBox().setupBBoxBuffers();
                entities.emplace_back(entity);
            }
        }
    }

}