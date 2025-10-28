#include "rendering/Renderer.h"
#include "Globals.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb/stb_image_write.h"

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

    for (size_t i = 0; i < lightEntities.size(); ++i) {
        if (lights[i].getType() != LightType::LIGHT_POINT)
            continue;

        Vec3 lightPos = lights[i].getPosition();
        lightEntities[i]->getTransform().setTranslation(lightPos);

        //TODO: Faire en sorte que la couleur dépende de la lumière active, pas forcément m_colorMeshLight
        scene.getLightingManager().applyPosLights(*m_lightShader);
        lightEntities[i]->draw_entity(*m_lightShader, view, projection);
    }
}

void Renderer::renderFrame(const Scene & scene) {
    m_shadowManager.renderShadows(scene);
    renderScene(scene);
}