#include "Globals.h"
#include "system/UtilsFile.h"

std::unique_ptr<Scene> g_scenePtr = nullptr;
Scene * g_scene = nullptr;
Renderer* g_renderer = nullptr;

void reloadScene(const std::size_t & index) {
    g_scenePtr = std::make_unique<Scene>(index);
    g_scene = g_scenePtr.get();
}
Scene & getScene() {
    assert(g_scene && "La scène globale n'est pas initialisée !");
    return *g_scene;
}


// Ses trois variables sont réellement initialisées via le render dans Application.cpp
std::string g_settingsFilePath;
float g_sensibility_depl;
float g_sensibility_rot;

size_t g_windowWidth = 1600;
size_t g_windowHeight = 800;

size_t g_sceneIndex = 0; // Scene par défaut qui va nécessairement être une erreur, doit être initialisée

float g_lightSize = 0.5f;

Vec3 g_selectedColor = Color::rose();

std::unordered_map<std::string, bool> g_entityExpanded;
bool g_forceOpenObjectHeader = false;

std::unordered_map<std::string, bool> g_lightExpanded;
bool g_forceOpenLightHeader = false;

TypeEntityCreated g_typeEntityCreated = TypeEntityCreated::Cube;
bool g_isCreatingEntity = true;

PerformanceStats g_perfStats = {};


void resetPerformancesStatsOnMeshesDraw() {
    g_perfStats.numberPointsRendered = 0;
    g_perfStats.numberTrianglesRendered = 0;
    g_perfStats.numberEntitiesDrawn = 0;
}

void updatePerformanceStatsOnAddedEntity(const Entity& entity) {
    g_perfStats.totalNumberEntitiesInScene++;

    for (const auto& sub : entity.getSubMeshes()) {
        for (const auto& chunk : sub.gridChunks) {
            g_perfStats.totalNumberPointsInScene += chunk.mesh->getNumberOfVertices();
            g_perfStats.totalNumberTrianglesInScene += chunk.mesh->getNumberOfIndices() / 3;
        }
    }
}

void updatePerformanceStatsOnRemovedEntity(const Entity& entity) {
    g_perfStats.totalNumberEntitiesInScene--;

    for (const auto& sub : entity.getSubMeshes()) {
        for (const auto& chunk : sub.gridChunks) {
            g_perfStats.totalNumberPointsInScene -= chunk.mesh->getNumberOfVertices();
            g_perfStats.totalNumberTrianglesInScene -= chunk.mesh->getNumberOfIndices() / 3;
        }
    }
}

void updatePerformanceStatsOnEntityDrawn(const Entity& entity) {
    g_perfStats.numberEntitiesDrawn++;

    for (const auto& sub : entity.getSubMeshes()) {
        for (const auto& chunk : sub.gridChunks) {
            g_perfStats.numberPointsRendered += chunk.mesh->getNumberOfVertices();
            g_perfStats.numberTrianglesRendered += chunk.mesh->getNumberOfIndices() / 3;
        }
    }
}
