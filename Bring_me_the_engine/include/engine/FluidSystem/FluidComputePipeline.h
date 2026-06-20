#pragma once

#include "rendering/Shader.h"
#include "engine/FluidSystem/ParticleBuffer.h"
#include "engine/FluidSystem/FluidConfig.h"
#include <memory>

class FluidComputePipeline
{
public:
    FluidComputePipeline() = default;

    void init(ParticleBuffer &buffer);

    void integrate(float dt, const FluidConfig &config);

private:
    ParticleBuffer *m_buffer = nullptr;

    std::unique_ptr<Shader> m_densityShader;
    std::unique_ptr<Shader> m_forcesShader;
    std::unique_ptr<Shader> m_integrateShader;
};