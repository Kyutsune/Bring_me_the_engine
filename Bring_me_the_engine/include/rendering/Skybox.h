#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>
#include "rendering/Shader.h"

/**
 * @brief Classe représentant une skybox dans le moteur.
 * 
 * Gère la création, le rendu et la liaison de la skybox via une cubemap OpenGL.
 */
class Skybox {
public:
    GLuint m_textureID;  ///< ID OpenGL de la cubemap.
    GLuint m_VAO, m_VBO; ///< Vertex Array Object et Vertex Buffer Object pour le cube de la skybox.

    /**
     * @brief Constructeur.
     * @param faces Liste des chemins des 6 textures de la cubemap (ordre : droite, gauche, haut, bas, avant, arrière).
     */
    Skybox(const std::vector<std::string> & faces);

    /**
     * @brief Destructeur.
     * Libère les ressources OpenGL allouées.
     */
    ~Skybox();

    /**
     * @brief Lie la texture de la skybox sur une unité de texture.
     * @param unit Unité de texture OpenGL (par défaut 0).
     */
    void bind(unsigned int unit = 0) const;

    /**
     * @brief Débind la skybox de l’unité de texture active.
     */
    void unbind() const;

    /**
     * @brief Initialise les buffers du cube (VAO/VBO) pour le rendu de la skybox.
     */
    void setupCube();

    /**
     * @brief Rendu de la skybox.
     * @param shader Shader utilisé pour le rendu.
     * @param view Matrice de vue.
     * @param projection Matrice de projection.
     */
    void draw(const Shader& shader, const Mat4& view, const Mat4& projection) const;
};
