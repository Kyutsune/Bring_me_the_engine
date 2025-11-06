#include "rendering/Renderer.h"
#include "Globals.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb/stb_image_write.h"

#include <numeric>
#include <deque>

Renderer::Renderer(Shader * entityShader, Shader * lightShader, Shader * skyboxShader, Shader * boundingBoxShader, Shader * shadowShaderDirectionnal, Shader * shadowShaderPonctual)
    : m_entityShader(entityShader),
      m_lightShader(lightShader),
      m_skyboxShader(skyboxShader),
      m_boundingBoxShader(boundingBoxShader),
      m_shadowShaderDirectionnal(shadowShaderDirectionnal),
      m_shadowShaderPonctual(shadowShaderPonctual),
      m_shadowManager(shadowShaderDirectionnal, shadowShaderPonctual) {
    initShadowMap();
}

void Renderer::renderScene(const Scene & scene) {
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

void Renderer::renderSkybox(const Skybox * skybox, const Mat4 & view, const Mat4 & projection) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->m_textureID);
    skybox->draw(*m_skyboxShader, view, projection);
}

void Renderer::renderEntities(const Scene & scene, const Mat4 & view, const Mat4 & projection) {
    const Frustum & frustum = scene.getFrustum();
    const std::vector<std::shared_ptr<Entity>> & entities = scene.getEntities();

    for (const std::shared_ptr<Entity> & entity : entities) {
        if (frustum.isBoxInFrustum(entity->getTransformedBoundingBox())) {
            entity->draw_entity(*m_entityShader, view, projection);
            entity->setVisible(true);
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
        if (lights[i].getType() != LightType::LIGHT_POINT && !frustum.isBoxInFrustum(lightEntities[i]->getTransformedBoundingBox()))
            continue;

        Vec3 lightPos = lights[i].getPosition();
        lightEntities[i]->getTransform().setTranslation(lightPos);

        scene.getLightingManager().applyPosLights(*m_lightShader, lights[i].getColor());
        lightEntities[i]->draw_entity(*m_lightShader, view, projection);
        lightEntities[i]->setVisible(true);
    }
}

void Renderer::renderFrame(const Scene & scene) {
    // Rendu des ombres
    m_shadowRenderTimer.start();
    m_shadowManager.renderShadows(scene);
    m_shadowRenderTimer.stop();

    // Rendu principal
    m_sceneRenderTimer.start();
    renderScene(scene);
    m_sceneRenderTimer.stop();

    // TODO: Rendre les résultats affichés optionnels à une variable qu'on peut activer/désactiver au clavier et dans Imgui
    // TODO: Rendre les résultats affichés plus jolis (dans Imgui ou alors à l'écran carrément dans le moteur(je préfère la deuxième solution))

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

        // Affichage console
        std::cout << "GPU Frame time: " << std::fixed << std::setprecision(3) << totalTimeMs << " ms"
                  << " [Ombres: " << shadowTimeMs << " ms, Scène: " << sceneTimeMs << " ms]"
                  << " (Avg: " << avgMs << " ms, " << gpuFps << " FPS)"
                  << " | CPU: " << cpuFrameTimeMs << " ms (" << cpuFps << " FPS)"
                  << " | FPS final: " << finalFps << " (Avg: " << avgFinalFps << " FPS)"

                  << std::endl;
    }
}
