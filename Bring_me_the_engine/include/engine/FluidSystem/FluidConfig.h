#pragma once

#include "math/Vec.h"


struct FluidConfig {
    float restDensity;
    float stiffness;
    float viscosity;
    float smoothingRadius;
    float particleMass;
    float particleRadius;
    float gravity;

    Vec3 boxMin;
    Vec3 boxMax;
};