#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "math/Vec.h"
#include "engine/Light.h"
#include "engine/LightSettings.h"

/**
 * @brief Représente un shader dans le moteur.
 * 
 * Cette classe compile et lie des shaders vertex, fragment et éventuellement geometry.
 * Elle fournit des fonctions utilitaires pour envoyer des uniforms au shader.
 */
class Shader {
public:
    GLuint ID; ///< Identifiant OpenGL du shader program.

    /**
     * @brief Constructeur.
     * 
     * Compile et lie les shaders vertex et fragment, et optionnellement un shader geometry.
     * @param vertexPath Chemin vers le fichier vertex shader.
     * @param fragmentPath Chemin vers le fichier fragment shader.
     * @param geometryPath Chemin vers le fichier geometry shader (optionnel).
     */
    Shader(const std::string & vertexPath, const std::string & fragmentPath, const std::string & geometryPath = "");

    /**
     * @brief Destructeur.
     * 
     * Supprime le shader program d'OpenGL.
     */
    ~Shader();

    /**
     * @brief Active le shader pour les appels de rendu suivants.
     */
    void use() const;

    // Fonctions pour définir des uniforms simples
    void setBool(const std::string & name, bool value) const;
    void setInt(const std::string & name, int value) const;
    void setFloat(const std::string & name, float value) const;

    // Fonctions pour définir des uniforms plus complexes
    void setMat4(const std::string & name, const Mat4 & mat) const;
    void setVec3(const std::string & name, float x, float y, float z) const;
    void setVec3(const std::string & name, const Vec3 & vec) const;

    // Fonctions génériques pour envoyer des uniforms
    void set(const std::string &name, const Vec3 &vec);
    void set(const std::string &name, float value);
    void set(const std::string &name, int value);
    void set(const std::string &name, const Mat4 &mat, const bool & should_transpose = false);

    /**
     * @brief Envoie un tableau de valeurs entières au shader.
     * 
     * @param name Nom de l'uniform.
     * @param values Pointeur vers le tableau.
     * @param count Nombre d'éléments dans le tableau.
     */
    void setArray(const std::string& name, const GLint* values, int count);

    void setVec3Array(const std::string& name, const glm::vec3* values, int count);
    void setFloatArray(const std::string& name, const float* values, int count);

private:
    /**
     * @brief Charge le contenu d'un fichier dans une string.
     * 
     * @param path Chemin du fichier.
     * @return Contenu du fichier sous forme de string.
     */
    std::string loadFile(const std::string & path) const;

    /**
     * @brief Compile un shader OpenGL.
     * 
     * @param type Type de shader (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER).
     * @param source Code source du shader.
     * @return ID du shader compilé.
     */
    GLuint compileShader(GLenum type, const std::string & source, const std::string & pathfile) const;
};
