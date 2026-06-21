#include "engine/FluidSystem/ParticleBuffer.h"
#include <vector>

float ParticleBuffer::init(int count, float spacing, const FluidConfig & config) {
    m_count = count;
    std::vector<Particle> data(count);

    int particlesPerAxis = std::ceil(std::cbrt(count));
    float blockSize = (particlesPerAxis - 1) * spacing;

    Vec3 boxCenter = (config.boxMin + config.boxMax) * 0.5f;
    Vec3 spawnStart(boxCenter.x - blockSize * 0.5f,
                    config.boxMin.y + 0.2f,
                    boxCenter.z - blockSize * 0.5f);

    int index = 0;
    for (int x = 0; x < particlesPerAxis && index < count; x++) {
        for (int y = 0; y < particlesPerAxis && index < count; y++) {
            for (int z = 0; z < particlesPerAxis && index < count; z++) {
                data[index].position = Vec4(spawnStart.x + x * spacing, spawnStart.y + y * spacing, spawnStart.z + z * spacing, 0.0f);
                data[index].velocity = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
                index++;
            }
        }
    }

    // Plus de calcul de calibration magique, on renvoie directement la masse de la config
    if (ssboA == 0) {
        glGenBuffers(1, &ssboA);
        glGenBuffers(1, &ssboB);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboA);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * count, data.data(), GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboB);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * count, data.data(), GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    useA = true;

    return config.particleMass;
}

void ParticleBuffer::bindRead() {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, useA ? ssboA : ssboB);
}

void ParticleBuffer::bindWrite() {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, useA ? ssboB : ssboA);
}

void ParticleBuffer::swap() {
    useA = !useA;
}

void ParticleBuffer::debugReadParticle(int index, const FluidConfig & config) {
    struct Particle {
        Vec4 position;
        Vec4 velocity;
    };

    Particle p;

    GLuint buffer = useA ? ssboA : ssboB;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);

    void * ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

    if (ptr) {
        Particle * particles = reinterpret_cast<Particle *>(ptr);

        p = particles[index];

        std::cout << "[DEBUG] Particule " << index
                  << " | Pos: " << p.position.x << ", " << p.position.y << ", " << p.position.z
                  << " | Densité: " << p.position.w
                  << " | Vitesse: " << p.velocity.x << ", " << p.velocity.y << ", " << p.velocity.z
                  << " | Pression: " << p.velocity.w << "\n";

        std::cout << "Tandis que voici la masse calculée dans config: " << config.particleMass << std::endl;

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    } else {
        std::cerr << "[ERROR] Impossible de mapper le buffer pour lecture.\n";
    }
}