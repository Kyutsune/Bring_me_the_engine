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

    m_gAlbedo = createTexture(GL_RGBA8, GL_COLOR_ATTACHMENT0);
    m_gNormal = createTexture(GL_RGB16F, GL_COLOR_ATTACHMENT1);

    glGenTextures(1, &m_gDepth);
    glBindTexture(GL_TEXTURE_2D, m_gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gDepth, 0);

    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Erreur : GBuffer FBO incomplet !" << std::endl;
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



    return true;
}

GLuint GBuffer::createTexture(GLenum format, GLenum attachment) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex, 0);
    return tex;
}

void GBuffer::release() {
    if (m_gAlbedo) { glDeleteTextures(1, &m_gAlbedo); m_gAlbedo = 0; }
    if (m_gNormal) { glDeleteTextures(1, &m_gNormal); m_gNormal = 0; }
    if (m_gDepth) { glDeleteTextures(1, &m_gDepth); m_gDepth = 0; }
    if (m_fbo) { glDeleteFramebuffers(1, &m_fbo); m_fbo = 0; }
}

void GBuffer::bindForWriting() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GBuffer::bindForReading() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedo);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gDepth);
}

int GBuffer::render(const Scene& scene, const Camera& camera, Shader & gBufferShader) {
    gBufferShader.use();
    bindForWriting();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    Transform view = camera.getViewMatrix();
    Transform proj = scene.getCamera().getProjectionMatrix();
    const std::vector<std::shared_ptr<Entity>>& entities = scene.getEntities();


    int drawnTriangles = 0;
    for (const std::shared_ptr<Entity>& entity : entities) {
        Transform model = entity->getTransform();
        Transform mvp = model * view * proj;
        gBufferShader.set("mvpMatrix", mvp, false);
        Transform normalMatrix = model.normal();
        gBufferShader.set("normalMatrix", normalMatrix);

        entity->getMesh()->draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return 0;
}



void GBuffer::renderDepth(const Scene& scene, const Camera& camera, Shader& depthShader) {
    bindForWriting();

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    depthShader.use();

    Mat4 view = camera.getViewMatrix();
    Mat4 proj = camera.getProjectionMatrix();

    const auto& entities = scene.getEntities();

    for (const auto& entity : entities) {
        Mat4 model = entity->getTransform();
        Mat4 mvp = proj * view * model;
        depthShader.set("mvpMatrix", mvp, true);

        entity->getMesh()->draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
