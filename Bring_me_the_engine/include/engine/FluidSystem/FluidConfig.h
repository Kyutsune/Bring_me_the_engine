#pragma once

#include "math/Vec.h"

struct FluidConfig {
    int numberOfParticles;
    float baseSpacing;
    float restDensity;
    float stiffness;
    float viscosity;
    float smoothingRadius;
    float particleMass;
    float particleRadius;
    float gravity;

    bool useBox;
    Vec3 boxMin;
    Vec3 boxMax;
};