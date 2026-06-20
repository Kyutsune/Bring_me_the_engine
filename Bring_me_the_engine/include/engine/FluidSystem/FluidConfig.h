#pragma once


struct FluidConfig {
    float restDensity;
    float stiffness;
    float viscosity;
    float smoothingRadius;
    float particleMass;
    float gravity;

    Vec3 boxMin;
    Vec3 boxMax;
};