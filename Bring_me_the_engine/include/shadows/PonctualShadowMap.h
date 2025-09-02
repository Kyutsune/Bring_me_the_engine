#pragma once
#include "engine/Scene.h"
#include "math/Vec.h"
#include "rendering/Shader.h"

/**
 * @brief Représente une shadow map pour une lumière ponctuelle (point light).
 * 
 * Cette classe gère une shadow map cubique (depth cubemap) qui stocke la profondeur
 * depuis la position de la lumière dans toutes les directions. Cela permet de simuler
 * des ombres projetées par des lumières ponctuelles dans toutes les directions.
 */
class PonctualShadowMap {
public:
    /**
     * @brief Constructeur par défaut.
     * Dimensions par défaut définies en interne.
     */
    PonctualShadowMap();

    /**
     * @brief Constructeur avec dimensions personnalisées.
     * @param width Largeur de la shadow map cubique.
     * @param height Hauteur de la shadow map cubique.
     */
    PonctualShadowMap(unsigned int width, unsigned int height);

    ~PonctualShadowMap() = default;

    /**
     * @brief Initialise la framebuffer et la depth cubemap pour la shadow map.
     */
    void init();

    /**
     * @brief Rendu de la shadow map pour une lumière ponctuelle donnée.
     * @param scene La scène à rendre.
     * @param shadowShader Shader utilisé pour le rendu de la profondeur.
     * @param pointLight Lumière ponctuelle à partir de laquelle générer la shadow map.
     */
    void render(const Scene & scene, Shader & shadowShader, const Light & pointLight);

    /**
     * @brief Lie la texture de la depth cubemap sur l'unité de texture spécifiée.
     * @param textureUnit Unité de texture OpenGL (GL_TEXTURE0, GL_TEXTURE1, etc.).
     */
    void bindTexture(GLenum textureUnit) const;

    /**
     * @brief Récupère la position de la lumière associée à cette shadow map.
     * @return Référence vers la position de la lumière.
     */
    const Vec3 & getLightPosition() const;

    // ---------------- Debug ----------------

    /**
     * @brief Initialise un quad fullscreen pour le rendu de debug de la shadow map.
     */
    void initDebugQuad();

    /**
     * @brief Affiche une face spécifique de la shadow map cubique pour le debug.
     * @param faceIndex Index de la face à afficher (0 à 5).
     */
    void debugRenderFace(int faceIndex);

private:
    unsigned int m_shadowFBO;      ///< Framebuffer OpenGL pour la shadow map.
    unsigned int m_depthCubemap;   ///< Texture cubique de profondeur.
    unsigned int m_width, m_height;///< Dimensions de la shadow map.
    Vec3 m_lightPosition;           ///< Position de la lumière ponctuelle.

    // IDs pour quad fullscreen (debug)
    GLuint m_debugQuadVAO;
    GLuint m_debugQuadVBO;

    // Shader utilisé pour le rendu debug du quad
    Shader * m_debugShader;
};
