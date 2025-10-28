#pragma once
#include "engine/Scene.h"
#include "math/Color.h"
#include <unordered_map>

// Pointeur global vers la scène actuelle, utilisé pour accéder à la caméra et aux entités
extern Scene * g_scene;
extern std::unique_ptr<Scene> g_scenePtr;
// entier non signé représentant l'index de la scène actuelle
extern size_t g_sceneIndex;

// Fonction qui va nous permettre de recharger la scène actuelle
void reloadScene(const std::size_t & index);
Scene & getScene();

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