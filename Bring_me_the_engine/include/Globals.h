#pragma once
#include "engine/Scene.h"
#include "rendering/Renderer.h"
#include "math/Color.h"
#include <unordered_map>

// Pointeur global vers la scène actuelle, utilisé pour accéder à la caméra et aux entités
extern Scene * g_scene;
extern std::unique_ptr<Scene> g_scenePtr;
// entier non signé représentant l'index de la scène actuelle
extern size_t g_sceneIndex;

extern Renderer * g_renderer;

// Fonction qui va nous permettre de recharger la scène actuelle
void reloadScene(const std::size_t & index);
Scene & getScene();

// Fichier contenant ses valeurs
extern std::string  g_settingsFilePath;
// Sensibilité de déplacement de la caméra via le clavier
extern float g_sensibility_depl;
// Sensibilité de rotation de la caméra via la souris
extern float g_sensibility_rot;

extern size_t g_windowWidth;
extern size_t g_windowHeight;

// Nombre maximum de lumières directionnelles et ponctuelles
inline constexpr size_t MAX_DIR_LIGHTS = 1;
inline constexpr size_t MAX_PONC_LIGHTS = 8;
inline constexpr size_t MAX_LIGHTS = MAX_DIR_LIGHTS + MAX_PONC_LIGHTS;

extern float g_lightSize;

// Partie qui nous servira à ouvrir ou non le menu quand on clique sur un objet
extern std::unordered_map<std::string, bool> g_entityExpanded;
// Pour forcer l'ouverture de la section des objets dans le menu
extern bool g_forceOpenObjectHeader;

// Même chose mais pour les lumières
extern std::unordered_map<std::string, bool> g_lightExpanded;
extern bool g_forceOpenLightHeader;

extern Vec3 g_selectedColor;

/// Partie qui gérera le fait de séléctionner quel entité on veut afficher dans le menu
enum TypeEntityCreated {
    None,
    Cube,
    Sphere,
    Plane
};

extern TypeEntityCreated g_typeEntityCreated;
extern bool g_isCreatingEntity;


struct PerformanceStats {
    double gpuFrameTimeMs;
    double gpuAvgMs;
    double gpuFps;
    double cpuFrameTimeMs;
    double cpuFps;
    double finalFps;
    double avgFinalFps;
    double numberPointsRendered;
	double numberTrianglesRendered;
    double numberEntitiesDrawn;
    double totalNumberPointsInScene;
	double totalNumberTrianglesInScene;
    double totalNumberEntitiesInScene;
};
extern PerformanceStats g_perfStats; 

extern void resetPerformancesStatsOnMeshesDraw();
extern void updatePerformanceStatsOnAddedEntity(const Entity & entity);
extern void updatePerformanceStatsOnRemovedEntity(const Entity& entity);
extern void updatePerformanceStatsOnEntityDrawn(const Entity& entity);