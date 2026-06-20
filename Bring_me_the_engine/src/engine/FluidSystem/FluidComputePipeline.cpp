#include "engine/FluidSystem/FluidComputePipeline.h"
#include "system/PathResolver.h"

void FluidComputePipeline::init(ParticleBuffer &buffer)
{
    m_buffer = &buffer;
    m_densityShader = std::make_unique<Shader>(PathResolver::getResourcePath("shaders/fluid/compute/density.comp"));
    m_integrateShader = std::make_unique<Shader>(PathResolver::getResourcePath("shaders/fluid/compute/integrate.comp"));
    std::cout << PathResolver::getResourcePath("shaders/fluid/compute/integrate.comp") << std::endl;
}

void FluidComputePipeline::integrate(float dt, const FluidConfig &config)
{
    int count = m_buffer->getCount();
    int numGroups = (count + 127) / 128;

    m_densityShader->use();
    m_buffer->bindRead();
    m_buffer->bindWrite();

    m_densityShader->set("smoothingRadius", config.smoothingRadius);
    m_densityShader->set("restDensity", config.restDensity);
    m_densityShader->set("stiffness", config.stiffness);
    m_densityShader->set("particleMass", config.particleMass);
    m_densityShader->set("particleCount", count);

    glDispatchCompute(numGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    m_buffer->swap();

    // Passe 2 : Calcul des forces et intégration
    m_integrateShader->use();
    m_buffer->bindRead();  
    m_buffer->bindWrite(); 

    m_integrateShader->set("smoothingRadius", config.smoothingRadius);
    m_integrateShader->set("viscosity", config.viscosity);
    m_integrateShader->set("particleMass", config.particleMass);
    m_integrateShader->set("dt", dt);
    m_integrateShader->set("gravity", config.gravity);
    m_integrateShader->set("particleCount", count);

    m_integrateShader->set("boxMin", config.boxMin);
    m_integrateShader->set("boxMax", config.boxMax);

    glDispatchCompute(numGroups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    
    m_buffer->swap(); 
}