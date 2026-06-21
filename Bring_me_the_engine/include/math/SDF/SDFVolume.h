#pragma once
#include <glad/glad.h>
#include <vector>
#include "math/Vec.h"

class SDFVolume {
public:
    SDFVolume() : m_textureID(0), m_resX(0), m_resY(0), m_resZ(0), m_min(0.0f), m_max(0.0f) {}
    ~SDFVolume() {
        if (m_textureID) glDeleteTextures(1, &m_textureID);
    }

    void buildTexture(const std::vector<float>& data, int rx, int ry, int rz) {
        m_resX = rx; m_resY = ry; m_resZ = rz;

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_3D, m_textureID);

        glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, m_resX, m_resY, m_resZ, 0, GL_RED, GL_FLOAT, data.data());

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_3D, 0);
    }

    GLuint getTextureID() const { return m_textureID; }

    void setLocalBounds(const Vec3& min, const Vec3& max) { 
        m_min = min; 
        m_max = max; 
    }
    Vec3 getLocalMin() const { return m_min; }
    Vec3 getLocalMax() const { return m_max; }

private:
    GLuint m_textureID;
    int m_resX, m_resY, m_resZ;
    
    Vec3 m_min;
    Vec3 m_max;
};