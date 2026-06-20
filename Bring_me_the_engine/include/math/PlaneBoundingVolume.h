#pragma once
#include "math/Vec.h"
#include "rendering/Shader.h"
#include <GLFW/glfw3.h>
#include <array>
#include <glad/glad.h>

/**
 * @brief Représente un plan dans l'espace 3D défini par une normale et une distance.
 *
 * L'équation du plan est : ax + by + cz + d = 0, où (a,b,c) est la normale et d la distance.
 */
struct Plane {
    Vec3 m_normal;    ///< Vecteur normal unitaire du plan
    float m_distance; ///< Distance signée du plan à l'origine (d dans l'équation)

    /**
     * @brief Constructeur par défaut : plan horizontal passant par l'origine.
     */
    inline Plane() : m_normal(Vec3(0.0f, 1.0f, 0.0f)), m_distance(0.0f) {}

    /**
     * @brief Constructeur à partir d'une normale et d'une distance.
     * @param normal Vecteur normal du plan (pas forcément normalisé).
     * @param distance Distance signée à l'origine.
     */
    inline Plane(const Vec3 & normal, float distance) : m_normal(normal), m_distance(distance) {}

    /**
     * @brief Constructeur à partir d'un point du plan et de sa normale.
     * @param point Point appartenant au plan.
     * @param normal Vecteur normal non nécessairement normalisé.
     */
    inline Plane(const Vec3 & point, const Vec3 & normal) {
        this->m_normal = normal.normalized();
        this->m_distance = -this->m_normal.dot(point);
    }

    /**
     * @brief Constructeur à partir de trois points du plan.
     * @param a Premier point.
     * @param b Deuxième point.
     * @param c Troisième point.
     */
    inline Plane(const Vec3 & a, const Vec3 & b, const Vec3 & c) {
        m_normal = (b - a).cross(c - a).normalized();
        m_distance = -m_normal.dot(a);
    }

    /**
     * @brief Calcule la distance signée d'un point au plan.
     * @param point Point à tester.
     * @return Distance signée (positive si devant le plan, négative sinon).
     */
    inline float distanceToPoint(const Vec3 & point) const {
        return m_normal.dot(point) + m_distance;
    }
};

/**
 * @brief Axis-Aligned Bounding Box (boîte englobante alignée aux axes).
 *
 * Définie par deux points : minimum et maximum.
 * Permet aussi la gestion OpenGL pour l'affichage du bounding box.
 */
struct AABB {
    Vec3 m_min; ///< Coin minimal (x_min, y_min, z_min)
    Vec3 m_max; ///< Coin maximal (x_max, y_max, z_max)

    unsigned int bboxVAO; ///< VAO OpenGL pour le bounding box
    unsigned int bboxVBO; ///< VBO OpenGL pour les sommets
    unsigned int bboxEBO; ///< EBO OpenGL pour les indices

    /**
     * @brief Constructeur par défaut (boîte nulle aux coordonnées (0,0,0)).
     */
    AABB() : m_min(Vec3(0.0f)), m_max(Vec3(0.0f)), bboxVAO(0), bboxVBO(0), bboxEBO(0) {}

    /**
     * @brief Constructeur avec coins min et max.
     * @param min Point minimum.
     * @param max Point maximum.
     */
    AABB(const Vec3 & min, const Vec3 & max) : m_min(min), m_max(max) {}

    /**
     * @brief Applique une transformation à la bounding box.
     *
     * Transforme les 8 coins par la matrice puis recalcule min et max.
     * @param matrix Matrice 4x4 de transformation.
     * @return Nouvelle AABB transformée.
     */
    AABB transform(const Mat4 & matrix) const;

    /**
     * @brief Initialise les buffers OpenGL pour dessiner la bounding box.
     */
    void setupBBoxBuffers();

    /**
     * @brief Dessine la bounding box à l'écran.
     * @param model Matrice de modèle
     * @param view Matrice de vue
     * @param projection Matrice de projection
     * @param shader Shader utilisé pour le rendu
     */
    void drawAABB(const Mat4 & model, const Mat4 & view, const Mat4 & projection, Shader & shader);

    /**
     * @brief Affichage en flux texte.
     */
    friend std::ostream & operator<<(std::ostream & os, const AABB & box) {
        os << "AABB(min: " << box.m_min << ", max: " << box.m_max << ")";
        return os;
    }

    /**
     * @brief Calcule le point d'intersection de trois plans (s'ils se coupent en un point).
     * @param p1 Premier plan.
     * @param p2 Deuxième plan.
     * @param p3 Troisième plan.
     * @return Point d'intersection.
     */
    static Vec3 intersectPlanes(const Plane & p1, const Plane & p2, const Plane & p3);

    /**
     * @brief Retourne les 8 coins de la bounding box.
     * @return Tableau contenant les 8 sommets.
     */
    inline std::array<Vec3, 8> getCorners() const {
        return {
            Vec3(m_min.x, m_min.y, m_min.z),
            Vec3(m_max.x, m_min.y, m_min.z),
            Vec3(m_max.x, m_max.y, m_min.z),
            Vec3(m_min.x, m_max.y, m_min.z),
            Vec3(m_min.x, m_min.y, m_max.z),
            Vec3(m_max.x, m_min.y, m_max.z),
            Vec3(m_max.x, m_max.y, m_max.z),
            Vec3(m_min.x, m_max.y, m_max.z)};
    }

    inline void merge(const AABB& other) {
        m_min.x = std::min(m_min.x, other.m_min.x);
        m_min.y = std::min(m_min.y, other.m_min.y);
        m_min.z = std::min(m_min.z, other.m_min.z);

        m_max.x = std::max(m_max.x, other.m_max.x);
        m_max.y = std::max(m_max.y, other.m_max.y);
        m_max.z = std::max(m_max.z, other.m_max.z);
    }
};
