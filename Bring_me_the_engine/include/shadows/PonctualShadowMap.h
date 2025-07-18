#pragma once
#include "engine/Scene.h"
#include "math/Vec.h"
#include "rendering/Shader.h"

class PonctualShadowMap {
public:
    PonctualShadowMap();
    PonctualShadowMap(unsigned int width, unsigned int height);
    ~PonctualShadowMap() = default;

    void init();
    void render(const Scene & scene, Shader & shadowShader, const Light & pointLight);

    void bindTexture(GLenum textureUnit) const;
    const Vec3 & getLightPosition() const;

    // Section de déboggage
    void initDebugQuad();
    void debugRenderFace(int faceIndex);

private:
    unsigned int m_shadowFBO;
    unsigned int m_depthCubemap;
    unsigned int m_width, m_height;
    Vec3 m_lightPosition;

    // IDs pour quad fullscreen (debug)
    GLuint m_debugQuadVAO;
    GLuint m_debugQuadVBO;

    // Shader debug simple
    Shader * m_debugShader;
};