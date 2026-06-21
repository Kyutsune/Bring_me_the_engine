#include "engine/FluidSystem/FluidRenderer.h"
#include "geometry/Sphere.h"
#include "system/PathResolver.h"

void FluidRenderer::init() {
    m_shader = std::make_unique<Shader>(
        PathResolver::getResourcePath("shaders/fluid/render/fluid_gbuffer.vert"),
        PathResolver::getResourcePath("shaders/fluid/render/fluid_gbuffer.frag"));

    // Génération de la sphère de base (basse résolution pour préserver les perfs)
    m_sphereMesh = createSphere<std::shared_ptr<Mesh>>(1.0f, 12, 12);
}

void FluidRenderer::render(const ParticleBuffer & buffer, const ObstacleBuffer & obstacles, const Camera & cam, const Mat4 & projection) {
    m_shader->use();

    m_shader->set("viewMatrix", cam.getViewMatrix(), false);
    m_shader->set("projectionMatrix", projection, false);

    // -----------------------------------------------------------------
    // PASSE 1 : DESSIN DES OBSTACLES (Sphères analytiques)
    // -----------------------------------------------------------------
    // On désactive l'indexation par instance pour le mode obstacle
    m_shader->set("isObstacleMode", true);
    m_shader->set("fluidColor", Vec3(0.8f, 0.2f, 0.2f)); // Un rouge bien visible pour l'obstacle

    for (const auto & obs : obstacles.obstacles) {
        if (obs.type == 0) { // SPHÈRE
            m_shader->set("obstaclePos", obs.position);
            m_shader->set("obstacleRadius", obs.size.x); // Le rayon est stocké dans size.x

            // On dessine une seule sphère classique
            m_sphereMesh->draw();
        }
    }

    // -----------------------------------------------------------------
    // PASSE 2 : DESSIN DES PARTICULES FLUIDES
    // -----------------------------------------------------------------
    if (buffer.getCount() == 0)
        return;

    m_shader->set("isObstacleMode", false);

    const_cast<ParticleBuffer &>(buffer).bindRead();

    m_shader->set("particleRadius", m_particleRadius);
    m_shader->set("fluidColor", m_fluidColor);

    // Appel direct via l'API du Mesh
    m_sphereMesh->drawInstanced(buffer.getCount());

    // TODO: Ajouter un compteur de particules rendues et de triangles rendus pour les stats de perf
    //  g_perfStats.numberPointsRendered += m_sphereMesh->getNumberOfVertices() * buffer.getCount();
    //  g_perfStats.numberTrianglesRendered += (m_sphereMesh->getNumberOfIndices() / 3) * buffer.getCount();
}

void FluidRenderer::setParticleRadius(float radius) {
    m_particleRadius = radius;
}