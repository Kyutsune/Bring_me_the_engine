#pragma once
#include <string>
#include <glad/glad.h>

class Texture {
public:
    Texture(const std::string& path, GLint wrapMode = GL_REPEAT, GLint filterMode = GL_NEAREST_MIPMAP_NEAREST, bool flip = true);
    ~Texture();

    void bind(unsigned int unit = 0) const;
    void unbind() const;
    unsigned int getID() const { return m_textureID; }

private:
    unsigned int m_textureID;
    int m_width, m_height, m_nrChannels;
};