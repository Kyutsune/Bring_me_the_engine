#include "shadows/PonctualShadowMap.h"
#include "Globals.h"
#include "math/Trigo.h"

PonctualShadowMap::PonctualShadowMap() : m_width(0), m_height(0), m_shadowFBO(0),
                                         m_depthCubemap(0), m_lightPosition(Vec3(0.0f, 0.0f, 0.0f)),
                                         m_debugQuadVAO(0), m_debugQuadVBO(0), m_debugShader(nullptr) {}

PonctualShadowMap::PonctualShadowMap(unsigned int width, unsigned int height)
    : m_width(width), m_height(height), m_lightPosition(0.0f, 0.0f, 0.0f) {
    init();
}

void PonctualShadowMap::init() {
    glGenFramebuffers(1, &m_shadowFBO);

    glGenTextures(1, &m_depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F,
                     m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubemap, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PonctualShadowMap::render(const Scene& scene, Shader& shadowShader, const Light& pointLight) {
    m_lightPosition = pointLight.getPosition();

    float nearPlane = 1.0f;
    float farPlane = pointLight.computeEffectiveRange(0.01f);

    Mat4 shadowProj = Mat4::perspective(radians(90.0f), 1.0f, nearPlane, farPlane);

    Vec3 pos = pointLight.getPosition();
    Mat4 shadowViews[6] = {
        Mat4::lookAt(pos, pos + Vec3(1, 0, 0), Vec3(0, -1, 0)),
        Mat4::lookAt(pos, pos + Vec3(-1, 0, 0), Vec3(0, -1, 0)),
        Mat4::lookAt(pos, pos + Vec3(0, 1, 0), Vec3(0, 0, 1)),
        Mat4::lookAt(pos, pos + Vec3(0, -1, 0), Vec3(0, 0, -1)),
        Mat4::lookAt(pos, pos + Vec3(0, 0, 1), Vec3(0, -1, 0)),
        Mat4::lookAt(pos, pos + Vec3(0, 0, -1), Vec3(0, -1, 0)) };

    glViewport(0, 0, m_width, m_height);
    shadowShader.use();

    shadowShader.set("lightPos", m_lightPosition);
    shadowShader.set("farPlane", farPlane);

    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    for (int face = 0; face < 6; ++face) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
            m_depthCubemap, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer incomplete for face " << face << std::endl;
            continue;
        }

        glClear(GL_DEPTH_BUFFER_BIT);

        Mat4 shadowMatrix = shadowViews[face] * shadowProj;
        shadowShader.set("shadowMatrix", shadowMatrix);

        for (const auto& entity : scene.getEntities()) {
            shadowShader.set("model", entity->getTransform(), false);

            // On parcourt chaque groupe de matériaux
            for (const auto& sub : entity->getSubMeshes()) {
                // On parcourt chaque cellule spatiale pour dessiner la géométrie
                for (const auto& cell : sub.gridChunks) {
                    cell.mesh->draw();
                }
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, g_windowWidth, g_windowHeight);
}

void PonctualShadowMap::bindTexture(GLenum textureUnit) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);
}