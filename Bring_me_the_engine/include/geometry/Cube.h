#pragma once
#include <vector>
#include <memory>
#include "engine/Mesh.h"
#include "math/Color.h"
#include "math/Tang_Bitang.h"

/**
 * @brief Helper template variable pour static_assert sur types non supportés.
 */
template <class>
inline constexpr bool always_false = false;

/**
 * @brief Crée un cube centré en (0,0,0) avec des dimensions 1x1x1.
 *
 * Version améliorée avec un vertex central par face pour une meilleure interpolation.
 * Chaque face est composée de 4 triangles partant du centre vers les coins,
 * ce qui permet une interpolation radiale correcte des couleurs vertex.
 *
 * @tparam T Type de retour, soit Mesh soit std::shared_ptr<Mesh>.
 * @param color Couleur uniforme appliquée à tous les sommets (par défaut gris clair).
 * @return Un objet de type T contenant le maillage du cube.
 */
template <typename T = Mesh>
inline T createCube(const Color & color = Color(204.f, 204.f, 204.f)) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Structure pour définir chaque face
    struct FaceData {
        Vec3 center;       // Centre de la face
        Vec3 normal;       // Normale de la face
        Vec3 corners[4];   // Les 4 coins dans l'ordre CCW
        Vec2 uvCenter;     // UV du centre
        Vec2 uvCorners[4]; // UV des coins
    };

    // Définition des 6 faces
    FaceData faces[6] = {
        // Face avant (z = +0.5)
        {
            {0.0f, 0.0f, 0.5f}, {0, 0, 1}, {{-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}}, {0.5f, 0.5f}, {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}},
        // Face arrière (z = -0.5)
        {
            {0.0f, 0.0f, -0.5f}, {0, 0, -1}, {{0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}}, {0.5f, 0.5f}, {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}},
        // Face gauche (x = -0.5)
        {
            {-0.5f, 0.0f, 0.0f}, {-1, 0, 0}, {{-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, -0.5f}}, {0.5f, 0.5f}, {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}},
        // Face droite (x = +0.5)
        {
            {0.5f, 0.0f, 0.0f}, {1, 0, 0}, {{0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}}, {0.5f, 0.5f}, {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}},
        // Face haut (y = +0.5)
        {
            {0.0f, 0.5f, 0.0f}, {0, 1, 0}, {{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}}, {0.5f, 0.5f}, {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}},
        // Face bas (y = -0.5)
        {
            {0.0f, -0.5f, 0.0f}, {0, -1, 0}, {{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}}, {0.5f, 0.5f}, {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}}};

    // Construire le mesh
    for (int f = 0; f < 6; f++) {
        const FaceData & face = faces[f];
        unsigned int baseIdx = vertices.size();

        // Ajouter le vertex central
        vertices.push_back({face.center,
                            face.normal,
                            {color.r, color.g, color.b},
                            face.uvCenter});

        // Ajouter les 4 coins
        for (int i = 0; i < 4; i++) {
            vertices.push_back({face.corners[i],
                                face.normal,
                                {color.r, color.g, color.b},
                                face.uvCorners[i]});
        }

        // Créer 4 triangles depuis le centre vers chaque edge
        for (int i = 0; i < 4; i++) {
            indices.push_back(baseIdx);                   // Centre
            indices.push_back(baseIdx + 1 + i);           // Coin actuel
            indices.push_back(baseIdx + 1 + (i + 1) % 4); // Coin suivant
        }
    }

    computeTangentsAndBitangents(vertices, indices);

    if constexpr (std::is_same_v<T, Mesh>) {
        return Mesh(vertices, indices);
    } else if constexpr (std::is_same_v<T, std::shared_ptr<Mesh>>) {
        return std::make_shared<Mesh>(vertices, indices);
    } else {
        static_assert(always_false<T>, "Unsupported template parameter for createCube");
    }
}