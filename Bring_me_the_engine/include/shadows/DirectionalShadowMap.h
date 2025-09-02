#pragma once

#include "math/Vec.h"
#include "engine/Scene.h"
#include "rendering/Shader.h"

/**
 * @brief Représente une shadow map directionnelle pour le rendu des ombres.
 * 
 * Cette classe gère la création d'une framebuffer et d'une texture pour stocker la profondeur
 * depuis la perspective d'une lumière directionnelle, ainsi que la matrice "light space" nécessaire
 * pour le rendu des ombres dans le shader.
 */
class DirectionalShadowMap {
public:
    /**
     * @brief Constructeur.
     * @param width Largeur de la shadow map (par défaut 2048).
     * @param height Hauteur de la shadow map (par défaut 2048).
     */
    DirectionalShadowMap(unsigned int width = 2048, unsigned int height = 2048);

    ~DirectionalShadowMap();

    /**
     * @brief Initialise la framebuffer et la texture de profondeur.
     */
    void init();

    /**
     * @brief Rendu de la shadow map à partir de la scène et du shader de profondeur.
     * @param scene La scène à rendre.
     * @param shadowShader Shader utilisé pour le rendu de la profondeur.
     */
    void render(const Scene& scene, Shader& shadowShader);

    /**
     * @brief Lie la texture de shadow map sur l'unité de texture spécifiée.
     * @param textureUnit L'unité de texture OpenGL (GL_TEXTURE0, GL_TEXTURE1, etc.).
     */
    void bindTexture(GLenum textureUnit) const;

    /**
     * @brief Récupère la matrice de transformation depuis l'espace monde vers l'espace lumière.
     * @return La matrice "light space" (projection * view de la lumière).
     */
    const Mat4& getLightSpaceMatrix() const { return m_lightSpaceMatrix; }

private:
    unsigned int m_shadowFBO;     ///< Framebuffer OpenGL pour la shadow map.
    unsigned int m_shadowMap;     ///< Texture de profondeur.
    unsigned int m_width, m_height; ///< Dimensions de la shadow map.

    Mat4 m_lightSpaceMatrix;      ///< Matrice projection * view de la lumière directionnelle.
};
