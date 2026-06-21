#pragma once

#include "engine/FluidSystem/FluidConfig.h"
#include "engine/FluidSystem/ParticleBuffer.h"
#include "rendering/Shader.h"
#include <memory>

class ObstacleBuffer;

class FluidComputePipeline {
public:
    FluidComputePipeline() = default;

    void init(ParticleBuffer & buffer);

    void integrate(float dt, const FluidConfig & config, const ObstacleBuffer & obstacleBuffer);

private:
    ParticleBuffer * m_buffer = nullptr;

    std::unique_ptr<Shader> m_densityShader;
    std::unique_ptr<Shader> m_forcesShader;
    std::unique_ptr<Shader> m_integrateShader;
};