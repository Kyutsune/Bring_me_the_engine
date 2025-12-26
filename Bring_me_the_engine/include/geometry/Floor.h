#pragma once
#include "math/Color.h"
#include "engine/Mesh.h"
#include "math/Tang_Bitang.h"
#include <memory>
#include <vector>

/**
 * @brief Crée un plan horizontal ("floor") centré à l'origine.
 * 
 * Génère un maillage carré avec une hauteur constante, des couleurs uniformes,
 * et des coordonnées UV répétées en fonction de la taille.
 * Calcule aussi les tangentes et bitangentes nécessaires pour le shading.
 * 
 * @tparam T Type de retour : Mesh ou std::shared_ptr<Mesh>.
 * @param size Taille du carré (largeur et profondeur).
 * @param height Hauteur Y du plan.
 * @param color Couleur appliquée uniformément aux sommets (par défaut vert).
 * @return Un objet de type T représentant le mesh généré.
 * 
 * @note La densité des texels est fixée à 0.5f pour répéter les UV.
 * @note Le plan est orienté horizontalement avec une normale (0,1,0).
 */
template <typename T = Mesh>
inline T createFloor(float size = 10.0f, float height = 0.0f, const Color & color = Color(16.f, 144.f, 48.f)) {
    float half = size / 2.0f;

    float texelDensity = 0.5f; 
    float repeat = size * texelDensity;

    std::vector<Vertex> vertices = {
        {{-half, height, -half}, {0, 1, 0}, {color.r, color.g, color.b}, {0.0f, 0.0f}},
        {{half, height, -half}, {0, 1, 0}, {color.r, color.g, color.b}, {repeat, 0.0f}},
        {{half, height, half}, {0, 1, 0}, {color.r, color.g, color.b}, {repeat, repeat}},
        {{-half, height, half}, {0, 1, 0}, {color.r, color.g, color.b}, {0.0f, repeat}},
    };

    std::vector<unsigned int> indices = {
        0, 2, 1,
        2, 0, 3
    };

    computeTangentsAndBitangents(vertices, indices);

    if constexpr (std::is_same_v<T, Mesh>) {
        return Mesh(vertices, indices);
    } else if constexpr (std::is_same_v<T, std::shared_ptr<Mesh>>) {
        return std::make_shared<Mesh>(vertices, indices);
    } else {
        static_assert(always_false<T>, "Unsupported template parameter for createGround");
    }
}
