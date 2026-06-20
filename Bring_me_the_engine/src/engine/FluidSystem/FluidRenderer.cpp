#include "engine/FluidSystem/FluidRenderer.h"
#include "system/PathResolver.h"
#include "geometry/Sphere.h"

void FluidRenderer::init()
{
    m_shader = std::make_unique<Shader>(
        PathResolver::getResourcePath("shaders/fluid/render/fluid_gbuffer.vert"),
        PathResolver::getResourcePath("shaders/fluid/render/fluid_gbuffer.frag"));

    // Génération de la sphère de base (basse résolution pour préserver les perfs)
    m_sphereMesh = createSphere<std::shared_ptr<Mesh>>(1.0f, 12, 12);
}

void FluidRenderer::render(const ParticleBuffer &buffer, const Camera &cam, const Mat4 &projection)
{
    if (buffer.getCount() == 0)
        return;

    m_shader->use();
    const_cast<ParticleBuffer &>(buffer).bindRead();

    m_shader->set("viewMatrix", cam.getViewMatrix(), false);
    m_shader->set("projectionMatrix", projection, false);
    m_shader->set("particleRadius", m_particleRadius);
    m_shader->set("fluidColor", m_fluidColor);

    // Appel direct via l'API du Mesh
    m_sphereMesh->drawInstanced(buffer.getCount());

    // TODO: Ajouter un compteur de particules rendues et de triangles rendus pour les stats de perf
    //  g_perfStats.numberPointsRendered += m_sphereMesh->getNumberOfVertices() * buffer.getCount();
    //  g_perfStats.numberTrianglesRendered += (m_sphereMesh->getNumberOfIndices() / 3) * buffer.getCount();
}

void FluidRenderer::setParticleRadius(float radius)
{
    m_particleRadius = radius;
}