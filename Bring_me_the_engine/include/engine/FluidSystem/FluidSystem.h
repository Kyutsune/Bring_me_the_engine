#pragma once

#include "camera/Camera.h"
#include "engine/FluidSystem/FluidComputePipeline.h"
#include "engine/FluidSystem/FluidConfig.h"
#include "engine/FluidSystem/FluidRenderer.h"
#include "engine/FluidSystem/ParticleBuffer.h"
#include "engine/FluidSystem/ObstacleFluid.h"

#include "engine/Scene.h"



/// @brief Module haut niveau du système de fluides, orchstrant le buffer de particules, le pipeline de calcul et le rendu.
class FluidSystem {
public:
    void init(int particleCount);
    void update(float dt, const Scene & scene);
    void render(const Camera & cam, const Mat4 & projection);

    void setParams(const FluidConfig & config);

private:
    ParticleBuffer m_buffer;
    FluidComputePipeline m_compute;
    FluidRenderer m_FluidRenderer;

    FluidConfig m_config;

    ObstacleBuffer m_obstacleBuffer;
};

