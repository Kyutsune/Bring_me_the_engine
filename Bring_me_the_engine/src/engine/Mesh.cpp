#include "engine/Mesh.h"
#include <cstddef> // pour offsetof

Mesh::Mesh(const std::vector<Vertex> & vertices, const std::vector<unsigned int> & indices)
    : m_vertices(vertices), m_indices(indices), m_VAO(0), m_VBO(0), m_EBO(0), m_indexCount(indices.size()) {
    setupMesh();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // Export des données des sommets
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

    // Export des données d'index
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_position));
    glEnableVertexAttribArray(0);

    // Normales
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_normal));
    glEnableVertexAttribArray(1);

    // Couleurs
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_color));
    glEnableVertexAttribArray(2);

    // TexCoords
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_texCoords));
    glEnableVertexAttribArray(3);

    // Tangentes
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_tangent));
    glEnableVertexAttribArray(4);

    // Bitangentes
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_bitangent));
    glEnableVertexAttribArray(5);

    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::upload() {
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(Vertex), m_vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

AABB Mesh::getBoundingBox() const {
    if (m_vertices.empty()) {
        return AABB(); // Retourne une boîte vide si aucun sommet n'est défini
    }

    Vec3 min = m_vertices[0].m_position;
    Vec3 max = m_vertices[0].m_position;

    for (const auto & vertex : m_vertices) {
        min.x = std::min(min.x, vertex.m_position.x);
        min.y = std::min(min.y, vertex.m_position.y);
        min.z = std::min(min.z, vertex.m_position.z);

        max.x = std::max(max.x, vertex.m_position.x);
        max.y = std::max(max.y, vertex.m_position.y);
        max.z = std::max(max.z, vertex.m_position.z);
    }

    return AABB(min, max);
}