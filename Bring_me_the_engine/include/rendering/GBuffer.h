#pragma once

#include <glad/glad.h>
#include <vector>
#include <math/Vec.h>
#include "camera/Camera.h"
#include "engine/Mesh.h"
#include "engine/Scene.h"
#include "system/PathResolver.h"

class GBuffer {
public:
    GBuffer();
    ~GBuffer();

    bool init();
    void release();

    void bindForWriting();
    void bindForWritingWithNoClear();
    void bindForReading(Shader & shader);

    GLuint getAlbedoTexture() const { return m_gAlbedo; }
    GLuint getNormalTexture() const { return m_gNormal; }
    GLuint getDepthTexture() const { return m_gDepth; }
    GLuint getSpecularTexture() const { return m_gSpecular; }

    int render(const Scene& scene, const Camera& camera, Shader & gBufferShader);
	void renderDepth(const Scene& scene, const Camera& camera, Shader& depthShader);

    void blitDepthToDefaultBuffer();

	Shader& getDebugDepthShader() { return m_debugDepthShader; }

private:
    int m_width, m_height;
    GLuint m_fbo = 0;
    GLuint m_gAlbedo = 0;
    GLuint m_gNormal = 0;
    GLuint m_gDepth = 0;
	GLuint m_gSpecular = 0;

    GLuint createTexture(GLenum format, GLenum attachment);



    Shader m_debugDepthShader = Shader(
            PathResolver::getResourcePath("shaders/debug/fullscreen/fullscreen.vert"),
            PathResolver::getResourcePath("shaders/debug/fullscreen/debug_depth.frag")
    );;
};
