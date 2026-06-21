#pragma once

#include <glad/glad.h>

#include "math/Vec.h"
#include <vector>

enum class ObstacleType : int {
    SPHERE = 0,
    BOX = 1
};

struct ObstacleData {
    Mat4 inverseModelMatrix; // Passe de l'espace Monde à l'espace Local de l'entité
    Vec3 boxMin;             // Borne min locale de l'SDF (avec son padding de 10%)
    int textureSlot;         // Le sampler index du tableau dans le shader (0, 1, 2...)
    Vec3 boxMax;             // Borne max locale de l'SDF
    float padding;           // Alignement std430 (4 octets) pour fermer le bloc
};

struct ObstacleBuffer {
    std::vector<ObstacleData> obstacles;
    GLuint ssbo = 0;
};