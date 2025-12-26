#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb/stb_image.h"

#include "rendering/GestionTextures/Texture.h"
#include <iostream>

Texture::Texture(const std::string& path, GLint wrapMode, GLint filterMode, bool flip) {
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (filterMode == GL_NEAREST || filterMode == GL_NEAREST_MIPMAP_NEAREST) ? GL_NEAREST : GL_LINEAR);

    // Charger l'image
    if (flip)
        stbi_set_flip_vertically_on_load(true);
    else
        stbi_set_flip_vertically_on_load(false);

    unsigned char * data = stbi_load(path.c_str(), &m_width, &m_height, &m_nrChannels, 0);

    if (data) {
        GLenum format = GL_RGB;
        if (m_nrChannels == 1)
            format = GL_RED;
        else if (m_nrChannels == 3)
            format = GL_RGB;
        else if (m_nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Erreur lors du chargement de la texture : " << path << "\n";
    }
    stbi_image_free(data);
}

Texture::~Texture() {
    glDeleteTextures(1, &m_textureID);
}

void Texture::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}