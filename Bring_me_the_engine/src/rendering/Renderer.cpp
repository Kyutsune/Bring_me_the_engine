#include "rendering/Renderer.h"
#include "Globals.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb/stb_image_write.h"

#include <deque>
#include <numeric>

Renderer::Renderer(Shader * entityShader, Shader * lightShader, Shader * skyboxShader, Shader * boundingBoxShader, Shader * shadowShaderDirectionnal,
                   Shader * shadowShaderPonctual, Shader * gBufferShader, Shader * deferredLightingShader)
    : m_entityShader(entityShader),
      m_lightShader(lightShader),
      m_skyboxShader(skyboxShader),
      m_boundingBoxShader(boundingBoxShader),
      m_shadowShaderDirectionnal(shadowShaderDirectionnal),
      m_shadowShaderPonctual(shadowShaderPonctual),
      m_shadowManager(shadowShaderDirectionnal, shadowShaderPonctual),
      m_gBufferShader(gBufferShader),
      m_deferredLightingShader(deferredLightingShader) {
    m_gBuffer.init();

    m_renderType = RenderType::DEFERRED;
    std::cout << "[RENDERER] Render Type init on " << (m_renderType == RenderType::FORWARD ? "FORWARD" : "DEFERRED") << " rendering" << std::endl;

    glGenVertexArrays(1, &fullscreenVAO);

    initShadowMap();

    m_fluidSystem = std::make_unique<FluidSystem>();
    m_fluidSystem->init(15000);
}

Renderer::~Renderer() {
    std::cout << "Destruction du Renderer" << std::endl;
    std::cout << "Performances moyennes durant l'exécution :" << std::endl;
    std::cout << " - GPU Frame time: " << std::fixed << std::setprecision(3) << g_perfStats.gpuAvgMs << " ms"
              << " (" << g_perfStats.gpuFps << " FPS)" << std::endl;
    std::cout << " - CPU Frame time: " << std::fixed << std::setprecision(3) << g_perfStats.cpuFrameTimeMs << " ms"
              << " (" << g_perfStats.cpuFps << " FPS)" << std::endl;
    std::cout << " - FPS final moyen: " << std::fixed << std::setprecision(2) << g_perfStats.avgFinalFps << " FPS" << std::endl;
};

void Renderer::renderSceneForward(const Scene & scene) {
    Mat4 view = scene.getCamera().getViewMatrix();
    Mat4 projection = scene.getCamera().getProjectionMatrix();

    // Skybox
    if (scene.getSkybox() && m_skyboxShader) {
        renderSkybox(scene.getSkybox(), view, projection);
    }

    // Shader principal
    m_entityShader->use();

    // Shadow manager : bind les ombres actives dans le shader
    m_shadowManager.bindShadows(*m_entityShader, scene);

    // Envoyer les lumières classiques
    scene.getLightingManager().applyLightning(*m_entityShader, scene.getCamera().getPosition());

    // Rendu des entités visibles
    renderEntities(scene, view, projection);

    // Dessin des entités représentant les lumières
    renderLightEntities(scene, view, projection);
}

void Renderer::renderSceneDeferred(const Scene & scene) {
    Mat4 view = scene.getCamera().getViewMatrix();
    Mat4 projection = scene.getCamera().getProjectionMatrix();

    // On va remplir notre GBuffer avec les entités de la scène
    m_gBuffer.render(scene, scene.getCamera(), *m_gBufferShader);

    // Rendu du système de fluides dans le GBuffer
    m_gBuffer.bindForWritingWithNoClear();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    m_fluidSystem->render(scene.getCamera(), projection);

    // A partir de la, on va réaliser la passe d'éclairage en utilisant le GBuffer via le shader d'éclairage différé
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_deferredLightingShader->use();

    // Shadow manager : bind les ombres actives dans le shader
    m_shadowManager.bindShadows(*m_deferredLightingShader, scene);

    // Envoi des lumières
    scene.getLightingManager().applyLightning(*m_deferredLightingShader, scene.getCamera().getPosition());

    // Bind des textures du GBuffer
    m_gBuffer.bindForReading(*m_deferredLightingShader);

    // Les derniers uniforms nécessaires au rendu
    m_deferredLightingShader->set("inverseView", view.inverse(), false);
    m_deferredLightingShader->set("inverseProjection", projection.inverse(), false);
    m_deferredLightingShader->set("camPos", scene.getCamera().getPosition());

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(fullscreenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    m_gBuffer.blitDepthToDefaultBuffer();

    // Skybox
    if (scene.getSkybox() && m_skyboxShader) {
        renderSkybox(scene.getSkybox(), view, projection);
    }
    // Dessin des entités représentant les lumières
    renderLightEntities(scene, view, projection);
}

void Renderer::renderSkybox(const Skybox * skybox, const Mat4 & view, const Mat4 & projection) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->m_textureID);
    skybox->draw(*m_skyboxShader, view, projection);
}

void Renderer::renderEntities(const Scene & scene, const Mat4 & view, const Mat4 & projection) {
    const Frustum & frustum = scene.getFrustum();
    const std::vector<std::shared_ptr<Entity>> & entities = scene.getEntities();

    for (const std::shared_ptr<Entity> & entity : entities) {
        // TODO: Ici on recalcule les bounding box transformée à chaque frame, ce qui est pas optimal
        //  On pourrait stocker la AABB(déjà fait) et ne la recalculer que si la transformation de l'entité change
        if (frustum.isBoxInFrustum(entity->getTransformedBoundingBox())) {
            entity->drawForward(*m_entityShader, view, projection);
            entity->setVisible(true);
            updatePerformanceStatsOnEntityDrawn(*entity);
        } else {
            entity->setVisible(false);
        }

        // else if(!frustum.isBoxInFrustum(entity->getTransformedBoundingBox()) && entity->getName()=="Cube_tout_bleu"){
        //     std::cout<< "Entité " << entity->getName() << " Pas dans le frustum." << std::endl;
        // }
    }
}

void Renderer::renderLightEntities(const Scene & scene, const Mat4 & view, const Mat4 & projection) {
    const auto & lights = scene.getLightingManager().getLights();
    const std::vector<std::shared_ptr<Entity>> & lightEntities = scene.getLightEntities();
    const Frustum & frustum = scene.getFrustum();

    for (size_t i = 0; i < lightEntities.size(); ++i) {
        if (lights[i].getType() != LightType::LIGHT_POINT || !frustum.isBoxInFrustum(lightEntities[i]->getTransformedBoundingBox())) {
            continue;
        }

        Vec3 lightPos = lights[i].getPosition();
        lightEntities[i]->getTransform().setTranslation(lightPos);

        scene.getLightingManager().applyPosLights(*m_lightShader, lights[i].getColor());
        lightEntities[i]->drawForward(*m_lightShader, view, projection);
        lightEntities[i]->setVisible(true);

        updatePerformanceStatsOnEntityDrawn(*lightEntities[i]);
    }
}

void Renderer::renderFrame(const Scene & scene) {
    // Rendu des ombres
    m_shadowRenderTimer.start();
    m_shadowManager.renderShadows(scene);
    m_shadowRenderTimer.stop();

    // Mise à jour du système de fluides
    m_fluidSystem->update(0.001f, scene);

    // Rendu principal
    m_sceneRenderTimer.start();

    resetPerformancesStatsOnMeshesDraw();

    if (m_renderType == RenderType::FORWARD)
        renderSceneForward(scene);
    else if (m_renderType == RenderType::DEFERRED)
        renderSceneDeferred(scene);

    m_sceneRenderTimer.stop();

    // m_fluidSystem->render(scene.getCamera());

    // TODO: Retirer tout ça du cout et le rajouter dans Imgui dans la partie "Performances"
    static bool debugPrinted = false;
    if (debugPrinted) {
        std::cout << "Points rendus: " << g_perfStats.numberPointsRendered
                  << " | Triangles rendus: " << g_perfStats.numberTrianglesRendered
                  << " | Entités rendus: " << g_perfStats.numberEntitiesDrawn << std::endl;

        std::cout << " | Nombre d'entités dans la scène: " << g_perfStats.totalNumberEntitiesInScene
                  << " | Nombre de Points totaux dans la scène" << g_perfStats.totalNumberPointsInScene
                  << " | Nombre de Triangles totaux dans la scène" << g_perfStats.totalNumberTrianglesInScene << std::endl;
    }

    // Script pour les temps de rendu GPU et CPU
    // Début mesure CPU totale
    static auto lastCpuStart = std::chrono::high_resolution_clock::now();
    auto cpuStart = std::chrono::high_resolution_clock::now();
    double cpuFrameTimeMs = std::chrono::duration<double, std::milli>(cpuStart - lastCpuStart).count();
    lastCpuStart = cpuStart;
    // Fin mesure CPU totale

    // Lecture des résultats
    double shadowTimeMs, sceneTimeMs;
    if (m_shadowRenderTimer.getElapsedTime(shadowTimeMs) &&
        m_sceneRenderTimer.getElapsedTime(sceneTimeMs)) {

        double totalTimeMs = shadowTimeMs + sceneTimeMs;

        // Stockage des temps dans un buffer glissant
        static std::deque<double> frameTimes;
        frameTimes.push_back(totalTimeMs);
        if (frameTimes.size() > 100) // garde les 100 dernières frames
            frameTimes.pop_front();

        // Moyenne glissante
        double avgMs = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0) / frameTimes.size();

        // FPS GPU
        double gpuFps = 1000.0 / avgMs;

        // FPS CPU (basé sur la durée entre deux frames)
        double cpuFps = 1000.0 / cpuFrameTimeMs;

        // FPS global (limité par le plus lent)
        double finalFps = std::min(cpuFps, gpuFps);

        // Stockage des FPS finaux dans un buffer glissant
        static std::deque<double> finalFpsBuffer;
        finalFpsBuffer.push_back(finalFps);
        if (finalFpsBuffer.size() > 100)
            finalFpsBuffer.pop_front();

        double avgFinalFps = std::accumulate(finalFpsBuffer.begin(), finalFpsBuffer.end(), 0.0) / finalFpsBuffer.size();

        // Mise à jour des statistiques globales
        g_perfStats.gpuFrameTimeMs = totalTimeMs;
        g_perfStats.gpuAvgMs = avgMs;
        g_perfStats.gpuFps = gpuFps;
        g_perfStats.cpuFrameTimeMs = cpuFrameTimeMs;
        g_perfStats.cpuFps = cpuFps;
        g_perfStats.finalFps = finalFps;
        g_perfStats.avgFinalFps = avgFinalFps;
    }
}
