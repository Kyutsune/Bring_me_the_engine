#pragma once
#include <glad/glad.h>
#include <vector>
#include "math/Vec.h"


class SDFVolume {
public:
    SDFVolume() : m_textureID(0), m_resX(0), m_resY(0), m_resZ(0) {}
    ~SDFVolume() {
        if (m_textureID) glDeleteTextures(1, &m_textureID);
    }

    // Génère la texture 3D OpenGL à partir d'une grille de floats
    void buildTexture(const std::vector<float>& data, int rx, int ry, int rz) {
        m_resX = rx; m_resY = ry; m_resZ = rz;

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_3D, m_textureID);

        // R32F est parfait : 1 canal de float 32 bits pour la distance
        glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, m_resX, m_resY, m_resZ, 0, GL_RED, GL_FLOAT, data.data());

        // LINEAR obligatoire pour que le GPU interpole les distances entre les voxels !
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // On clamp aux bords pour éviter que le fluide ne rebondisse à l'infini en dehors de la boîte
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_3D, 0);
    }

    GLuint getTextureID() const { return m_textureID; }

private:
    GLuint m_textureID;
    int m_resX, m_resY, m_resZ;
};