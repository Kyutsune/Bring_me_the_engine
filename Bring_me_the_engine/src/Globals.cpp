#include "Globals.h"

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

float g_sensibility_depl = 0.20f;
float g_sensibility_rot = 0.01f;

size_t g_windowWidth = 1600;
size_t g_windowHeight = 800;

size_t g_sceneIndex = 0; // Scene par défaut qui va nécessairement être une erreur, doit être initialisée

float g_lightSize = 0.5f;

Vec3 g_selectedColor = Color::rose();

std::unordered_map<std::string, bool> g_entityExpanded;
bool g_forceOpenObjectHeader = false; 

TypeEntityCreated g_typeEntityCreated = TypeEntityCreated::Cube;
bool g_isCreatingEntity = true;

PerformanceStats g_perfStats = {};