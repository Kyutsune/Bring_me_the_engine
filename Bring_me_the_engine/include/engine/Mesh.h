#pragma once

#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "rendering/Vertex.h"
#include "rendering/GestionTextures/Texture.h"
#include "math/PlaneBoundingVolume.h"

/**
 * @class Mesh
 * @brief Représente un objet 3D composé de sommets et d'indices.
 *
 * La classe gère la création des buffers OpenGL (VAO, VBO, EBO) et fournit
 * les méthodes pour dessiner le mesh et obtenir son bounding box.
 */
class Mesh {
public:
    /**
     * @brief Constructeur à partir de listes de vertices et d'indices.
     * @param vertices Sommets du mesh
     * @param indices Indices pour dessiner les triangles
     */
    Mesh(const std::vector<Vertex> & vertices, const std::vector<unsigned int> & indices);

    /// Destructeur qui libère les ressources OpenGL
    ~Mesh();

    /// Dessine le mesh avec les buffers OpenGL configurés
    void draw() const;

    /// Dessine le mesh en utilisant l'instanciation OpenGL
    void drawInstanced(int instanceCount) const;

    /// Ré-envoie les données des sommets au GPU (utile après modification CPU)
    void upload();

    /// Retourne l'AABB qui englobe le mesh
    AABB getBoundingBox() const;

    /// Retourne une copie des vertices
    inline std::vector<Vertex> getVertices() const { return m_vertices; }

    /// Retourne une référence constante aux vertices
    inline std::vector<Vertex> & getVerticesRef() { return m_vertices; }

    /// Retourne une référence à un vertex
    inline Vertex & getVertexRef(size_t index) { return m_vertices.at(index); }

    /// Retourne une référence aux indices
    inline const std::vector<unsigned int> & getIndices() const { return m_indices; }

	inline const int getNumberOfVertices() const { return static_cast<int>(m_vertices.size()); }
	inline const int getNumberOfIndices() const { return static_cast<int>(m_indexCount); }

private:
    /// Vertex Array Object OpenGL
    GLuint m_VAO;

    /// Vertex Buffer Object OpenGL
    GLuint m_VBO;

    /// Element Buffer Object OpenGL
    GLuint m_EBO;

    /// Nombre d'indices pour le dessin
    GLsizei m_indexCount;

    /// Initialise les buffers OpenGL et configure les attributs de vertex
    void setupMesh();

    /// Liste des vertices du mesh
    std::vector<Vertex> m_vertices;

    /// Liste des indices pour dessiner les triangles
    std::vector<unsigned int> m_indices;
};
