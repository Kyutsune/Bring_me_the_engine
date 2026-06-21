#pragma once

#include <glad/glad.h>

#include "math/Vec.h"
#include <vector>


enum class ObstacleType : int {
    SPHERE = 0,
    BOX = 1
};

struct ObstacleData {
    Vec3 position; // Centre de la sphère ou de la boîte
    int type;      // ObstacleType casté en int

    Vec3 size;     // Rayon pour la sphère (size.x), ou demi-extensions (half-extents) pour la boîte
    float padding; // Alignement pour remplir le slot de 4 floats (std430)
};

struct ObstacleBuffer{
    std::vector<ObstacleData> obstacles;
    GLuint ssbo = 0;
};