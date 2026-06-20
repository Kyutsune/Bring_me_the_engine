#pragma once
#include "engine/Mesh.h"
#include "math/Color.h"
#include "math/Tang_Bitang.h"
#include <memory>
#include <type_traits>
#include <vector>

/**
 * @brief Crée une sphère paramétrique centrée à l'origine.
 *
 * Cette fonction génère une sphère approximative à partir de secteurs (longitude) et stacks (latitude).
 * Chaque vertex contient position, normale, couleur et coordonnées de texture.
 * Les tangentes et bitangentes sont calculées pour support des normal maps.
 *
 * @tparam T Type retourné : Mesh ou std::shared_ptr<Mesh>.
 * @param radius Rayon de la sphère.
 * @param sectorCount Nombre de divisions en longitude (secteurs).
 * @param stackCount Nombre de divisions en latitude (stacks).
 * @param color Couleur appliquée uniformément sur tous les vertices.
 * @return T L'objet mesh représentant la sphère.
 */
template <typename T = Mesh,
          typename = std::enable_if_t<std::is_same_v<T, Mesh> || std::is_same_v<T, std::shared_ptr<Mesh>>>>
inline T createSphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18, const Color & color = Color(255, 255, 255)) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float PI = 3.14159265359f;
    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = PI / 2 - i * stackStep; // de pi/2 à -pi/2
        float xy = radius * cosf(stackAngle);      // r * cos(u)
        float z = radius * sinf(stackAngle);       // r * sin(u)

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * sectorStep; // de 0 à 2pi

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            Vec3 position = Vec3(x, y, z);
            Vec3 normal = normalize(position); // normal = position sur la sphère
            Vec2 texCoords = Vec2((float)j / sectorCount, (float)i / stackCount);

            vertices.push_back(Vertex{
                position,
                normal,
                {color.r, color.g, color.b},
                texCoords});
        }
    }

    // indices
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1); // début de la rangée actuelle
        int k2 = k1 + sectorCount + 1;  // début de la rangée suivante

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    computeTangentsAndBitangents(vertices, indices);

    if constexpr (std::is_same_v<T, Mesh>) {
        return Mesh(vertices, indices);
    } else {
        return std::make_shared<Mesh>(vertices, indices);
    }
}