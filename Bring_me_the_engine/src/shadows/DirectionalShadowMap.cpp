#include "shadows/DirectionalShadowMap.h"
#include "Globals.h"
#include <glad/glad.h>
#include <iostream>

DirectionalShadowMap::DirectionalShadowMap(unsigned int width, unsigned int height)
    : m_width(width), m_height(height), m_shadowFBO(0), m_shadowMap(0) {}

DirectionalShadowMap::~DirectionalShadowMap() {
    if (m_shadowMap) glDeleteTextures(1, &m_shadowMap);
    if (m_shadowFBO) glDeleteFramebuffers(1, &m_shadowFBO);
}

void DirectionalShadowMap::init() {
    glGenFramebuffers(1, &m_shadowFBO);

    glGenTextures(1, &m_shadowMap);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, m_shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Erreur: Shadow framebuffer non complet !" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DirectionalShadowMap::render(const Scene& scene, Shader& shadowShader) {
    const Light& dirLight = scene.getLightingManager().getFirstDirectional();
    if (dirLight.getType() == LightType::LIGHT_ERROR || !dirLight.isActive())
        return;

    // --- Calcul des matrices de vue et projection (inchangé) ---
    Vec3 lightTarget = dirLight.getPosition() + dirLight.getDirection();
    Vec3 up = std::abs(dirLight.getDirection().z) > 0.9f ? Vec3(0, 1, 0) : Vec3(0, 0, 1);
    Mat4 lightView = Mat4::lookAt(dirLight.getPosition(), lightTarget, up);

    Camera lightCam(dirLight.getPosition(), lightTarget, up, 90.0f, 1.0f, 1.0f, 10.0f);
    Frustum lightFrustum = Frustum().updateFromCamera(lightCam);
    AABB boundingBox = lightFrustum.computeBoundingBox();

    Mat4 lightProjection = Mat4::orthographic(
        boundingBox.m_min.x, boundingBox.m_max.x,
        boundingBox.m_min.y, boundingBox.m_max.y,
        boundingBox.m_min.z, boundingBox.m_max.z);

    m_lightSpaceMatrix = lightView * lightProjection;

    shadowShader.use();
    shadowShader.set("lightSpaceMatrix", m_lightSpaceMatrix, false);

    glViewport(0, 0, m_width, m_height);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& entity : scene.getEntities()) {
        if (entity->getName() == "Sol_beton") continue;

        Mat4 model = entity->getTransform();
        shadowShader.set("model", model, false);

        for (const auto& sub : entity->getSubMeshes()) {

            for (const auto& cell : sub.gridChunks) {

                AABB cellWorldBox = cell.localAABB.transform(model);
                if (lightFrustum.isBoxInFrustum(cellWorldBox)) {
                    cell.mesh->draw();
                }
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, g_windowWidth, g_windowHeight);
}
    
void DirectionalShadowMap::bindTexture(GLenum textureUnit) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
}
