#pragma once
#include "math/Vec.h"

/**
 * @brief Représente un sommet d'un mesh.
 * 
 * Contient toutes les informations nécessaires pour le rendu avec OpenGL :
 * position, normales, couleur, coordonnées de texture, et tangentes/bitangentes pour le shading avancé.
 */
struct Vertex {
    Vec3 m_position;   ///< Position du sommet dans l'espace local.
    Vec3 m_normal;     ///< Normale pour l'éclairage.
    Vec3 m_color;      ///< Couleur du sommet (RGB).
    Vec2 m_texCoords;  ///< Coordonnées de texture (UV).
    Vec3 m_tangent;    ///< Tangente pour le calcul de normal mapping.
    Vec3 m_bitangent;  ///< Bitangente pour le calcul de normal mapping.
};
