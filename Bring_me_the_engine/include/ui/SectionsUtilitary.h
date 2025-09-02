#pragma once

#include <string>
#include <functional>
#include "math/Vec.h"
#include "math/Color.h"
#include "engine/Scene.h"
#include "engine/LightingManager.h"

namespace SectionsUtilitary {

    /**
     * @brief Affiche un éditeur de position dans l'UI.
     * 
     * Cette fonction génère un petit panneau d'édition pour une variable Vec3,
     * permettant de modifier les coordonnées X, Y et Z d'une entité ou d'une lumière
     * directement depuis le menu. Chaque modification déclenche le callback fourni.
     * 
     * @param labelPrefix Préfixe utilisé pour le label de l'UI (permet d'identifier le champ)
     * @param position Référence à la position Vec3 à éditer
     * @param onPositionChanged Callback appelé à chaque modification de la position, reçoit la nouvelle position
     */
    void renderPositionEditor(const std::string & labelPrefix, Vec3 & position, const std::function<void(const Vec3 &)> & onPositionChanged);

}
