#pragma once
#include "engine/Mesh.h"
#include "rendering/Shader.h"
#include "camera/Camera.h"
#include "engine/FluidSystem/ParticleBuffer.h"
#include <memory>

class FluidRenderer {
public:
    FluidRenderer() = default;
    ~FluidRenderer() = default;

    void init();
    void render(const ParticleBuffer& buffer, const Camera& cam, const Mat4& projection);

    void setParticleRadius(float radius);

private:
    std::unique_ptr<Shader> m_shader;
    std::shared_ptr<Mesh> m_sphereMesh;
    float m_particleRadius = 0.1f;
    Vec3 m_fluidColor = Vec3(0.0f, 0.4f, 0.8f); // Un joli bleu fluide (dans l'idée)
};