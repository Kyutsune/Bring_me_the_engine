#include "rendering/GBuffer.h"
#include "engine/Entity.h"
#include "Globals.h"
#include <iostream>

GBuffer::GBuffer()
    : m_width(g_windowWidth), m_height(g_windowHeight) {
}

GBuffer::~GBuffer() {
    release();
}

bool GBuffer::init() {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    m_gAlbedo = createTexture(GL_RGBA16F, GL_COLOR_ATTACHMENT0);
    m_gNormal = createTexture(GL_RGBA16F, GL_COLOR_ATTACHMENT1);
	m_gSpecular = createTexture(GL_RGBA16F, GL_COLOR_ATTACHMENT2);

    glGenTextures(1, &m_gDepth);
    glBindTexture(GL_TEXTURE_2D, m_gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gDepth, 0);

    GLenum attachments[3] = {
        GL_COLOR_ATTACHMENT0, // albedo
        GL_COLOR_ATTACHMENT1, // normal
        GL_COLOR_ATTACHMENT2  // specular
    };
    glDrawBuffers(3, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Erreur : GBuffer FBO incomplet !" << std::endl;
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    return true;
}

GLuint GBuffer::createTexture(GLenum internalFormat, GLenum attachment) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;

    if (internalFormat == GL_RGB16F || internalFormat == GL_RGBA16F) {
        format = (internalFormat == GL_RGBA16F) ? GL_RGBA : GL_RGB;
        type = GL_FLOAT;
    }   

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, type, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex, 0);

    return tex;
}

void GBuffer::release() {
    if (m_gAlbedo) { glDeleteTextures(1, &m_gAlbedo); m_gAlbedo = 0; }
    if (m_gNormal) { glDeleteTextures(1, &m_gNormal); m_gNormal = 0; }
    if (m_gDepth) { glDeleteTextures(1, &m_gDepth); m_gDepth = 0; }
	if (m_gSpecular) { glDeleteTextures(1, &m_gSpecular); m_gSpecular = 0; }
    if (m_fbo) { glDeleteFramebuffers(1, &m_fbo); m_fbo = 0; }
}

void GBuffer::bindForWriting() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GBuffer::bindForReading(Shader & shader) {
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_gDepth); shader.set("gDepth", 0);
	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, m_gAlbedo); shader.set("gAlbedo", 1);
	glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, m_gNormal); shader.set("gNormal", 2);
	glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, m_gSpecular); shader.set("gSpecular", 3);
}

int GBuffer::render(const Scene& scene, const Camera& camera, Shader & gBufferShader) {
    gBufferShader.use();
    bindForWriting();


    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    Transform view = camera.getViewMatrix();
    Transform proj = scene.getCamera().getProjectionMatrix();
    const std::vector<std::shared_ptr<Entity>>& entities = scene.getEntities();
    const Frustum& frustum = scene.getFrustum();


    int drawnTriangles = 0;
    for (const std::shared_ptr<Entity>& entity : entities) {
        if (frustum.isBoxInFrustum(entity->getTransformedBoundingBox())) {
            Transform model = entity->getTransform();

            Transform mvp = model * view * proj;
            gBufferShader.set("mvpMatrix", mvp, false);
            gBufferShader.set("modelMatrix", model, false);

            gBufferShader.set("baseColor", entity->getMaterial().m_baseColor);
            gBufferShader.set("useVertexColor", entity->getMaterial().m_useVertexColor);

            // Texture diffuse
            if (entity->hasTextureDiffuse() && entity->doItUseTextureDiffuse()) {
                gBufferShader.set("useTexture", 1);
                gBufferShader.set("albedoMap", 0);
                entity->getMaterial().m_diffuseTexture->bind(0);
            }
            else {
                gBufferShader.set("useTexture", 0);
            }

            // Normal map
            if (entity->hasNormalMap() && entity->doItUseNormalMap()) {
                gBufferShader.set("useNormalMap", true);
                gBufferShader.set("normalMap", 1);
                entity->getMaterial().m_normalMap->bind(1);
            }
            else {
                gBufferShader.set("useNormalMap", false);
            }

            // Specular map
            if (entity->hasSpecularMap() && entity->doItUseSpecularMap()) {
                gBufferShader.set("useSpecularMap", true);
                gBufferShader.set("specularMap", 2);
                entity->getMaterial().m_specularMap->bind(2);
            }
            else {
                gBufferShader.set("useSpecularMap", false);
            }


            entity->getMesh()->draw();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return 0;
}

void GBuffer::blitDepthToDefaultBuffer() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// On copie la depht calculée vers l'écran
    glBlitFramebuffer(
        0, 0, m_width, m_height,
        0, 0, m_width, m_height,
        GL_DEPTH_BUFFER_BIT,
        GL_NEAREST
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
