#pragma once

#include "math/Vec.h"
#include "engine/FluidSystem/FluidConfig.h"

#include <glad/glad.h>

struct Particle {
    Vec4 position; // xyz: position, w: masse ou densité
    Vec4 velocity; // xyz: vitesse,  w: pression
};

class ParticleBuffer {
public:
    float init(int count, float spacing, const FluidConfig& config);

    void bindRead();
    void bindWrite();

    void swap();

    int getCount() const { return m_count; }

    void debugReadParticle(int index, const FluidConfig& config);

    bool getUseA() const { return useA; }

private:
    GLuint ssboA = 0;
    GLuint ssboB = 0;

    int m_count = 0;
    bool useA = true;
};