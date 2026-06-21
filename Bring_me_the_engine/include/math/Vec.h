#pragma once

#include "math/Color.h"
#include <cmath>
#include <ostream>

// TODO: Mettre de l'ordre dans ce bordel.. pas forcément séparer en deux fichiers car... pourquoi? ça à du sens ensemble mais au moins bien foutre
//  Les déclarations dans le cpp

/**
 * @brief Vecteur 2D avec composantes flottantes.
 */
struct Vec2 {
    float x; ///< Composante X
    float y; ///< Composante Y

    /**
     * @brief Constructeur par défaut et avec initialisation des composantes.
     * @param x Composante X (défaut 0)
     * @param y Composante Y (défaut 0)
     */
    Vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

    Vec2 operator+(const Vec2 & other) const { return {x + other.x, y + other.y}; }
    Vec2 operator-(const Vec2 & other) const { return {x - other.x, y - other.y}; }
    Vec2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
    Vec2 operator/(float scalar) const { return {x / scalar, y / scalar}; }

    /**
     * @brief Accès au tableau de composantes (const).
     * @return Pointeur constant vers x.
     */
    const float * ptr() const { return &x; }

    /**
     * @brief Accès au tableau de composantes (modifiable).
     * @return Pointeur vers x.
     */
    float * ptr() { return &x; }

    /**
     * @brief Calcul de la longueur euclidienne du vecteur.
     * @return Longueur positive.
     */
    float length() const { return sqrt(x * x + y * y); }

    /**
     * @brief Retourne un vecteur normalisé (longueur 1).
     * Si la longueur est nulle, retourne une copie identique.
     * @return Vecteur normalisé.
     */
    Vec2 normalized() const {
        float len = length();
        return len == 0 ? *this : *this / len;
    }

    /**
     * @brief Opposé du vecteur.
     * @return Vecteur avec composantes inversées.
     */
    Vec2 operator-() const {
        return Vec2(-x, -y);
    }

    /**
     * @brief Affiche le vecteur au format Vec2(x, y).
     */
    friend std::ostream & operator<<(std::ostream & os, const Vec2 & v) {
        return os << "Vec2(" << v.x << ", " << v.y << ")";
    }
};

/**
 * @brief Vecteur 3D avec composantes flottantes.
 */
struct Vec3 {
    float x; ///< Composante X
    float y; ///< Composante Y
    float z; ///< Composante Z

    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
    Vec3(const Color & color);

    Vec3 operator+(const Vec3 & other) const;
    Vec3 operator-(const Vec3 & other) const;
    Vec3 operator*(float scalar) const;
    Vec3 operator*(const Vec3 & other) const;
    Vec3 operator/(float scalar) const;

    bool operator==(const Vec3 & other) const;
    bool operator!=(const Vec3 & other) const;
    Vec3 operator+=(const Vec3 & other);
    Vec3 operator-=(const Vec3 & other);

    Vec3 operator-() const;

    const float * ptr() const;
    float * ptr();

    float length() const;
    float lengthSq() const;
    Vec3 normalized() const;

    /**
     * @brief Produit scalaire entre deux vecteurs.
     * @param other Autre vecteur.
     * @return Valeur scalaire.
     */
    float dot(const Vec3 & other) const;

    /**
     * @brief Produit vectoriel entre deux vecteurs.
     * @param other Autre vecteur.
     * @return Vecteur perpendiculaire aux deux vecteurs.
     */
    Vec3 cross(const Vec3 & other) const;

    friend Vec3 operator*(float scalar, const Vec3 & v);
    friend std::ostream & operator<<(std::ostream & os, const Vec3 & v);
};

/**
 * @brief Normalise un vecteur 3D.
 * @param v Vecteur à normaliser.
 * @return Vecteur normalisé ou vecteur original si longueur nulle.
 */
inline Vec3 normalize(Vec3 v) {
    float len = v.length();
    if (len != 0.0f) {
        v = v / len;
    }
    return v;
}

/**
 * @brief Vecteur 4D avec composantes flottantes.
 */
struct Vec4 {
    float x, y, z, w;

    Vec4(float x = 0, float y = 0, float z = 0, float w = 0)
        : x(x), y(y), z(z), w(w) {}

    Vec4(const Vec3 & v, float w = 1.0f) {
        x = v.x;
        y = v.y;
        z = v.z;
        this->w = w;
    }

    Vec4 operator+(const Vec4 & o) const { return {x + o.x, y + o.y, z + o.z, w + o.w}; }
    Vec4 operator-(const Vec4 & o) const { return {x - o.x, y - o.y, z - o.z, w - o.w}; }
    Vec4 operator*(float s) const { return {x * s, y * s, z * s, w * s}; }
    Vec4 operator/(float s) const { return {x / s, y / s, z / s, w / s}; }
    Vec4 & operator/=(float s) {
        x /= s;
        y /= s;
        z /= s;
        w /= s;
        return *this;
    }
    

    const float * ptr() const { return &x; }
    float * ptr() { return &x; }

    Vec4 operator-() const {
        return Vec4(-x, -y, -z, -w);
    }

    friend std::ostream & operator<<(std::ostream & os, const Vec4 & v) {
        return os << "Vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    }
};

/**
 * @brief Matrice 4x4 stockée en ligne majeure.
 * 
 * Remarques importantes :  
 * - Les translations sont dans data[12], data[13], data[14].  
 * - Concrètement m_transform = S * R * T;  fait S puis R et finalement T, pas l'inverse (T puis R puis S) comme en glsl
 */
struct Mat4 {
    float data[16]; ///< Données matricielles stockées en ligne majeure.

    Mat4();
    Mat4(std::initializer_list<float> list);

    static Mat4 identity();
    static Mat4 lookAt(const Vec3 & eye, const Vec3 & center, const Vec3 & up);
    static Mat4 perspective(float fovRadians, float aspect, float near, float far);
    static Mat4 rotateX(float angleRadians);
    static Mat4 rotateY(float angleRadians);
    static Mat4 rotateZ(float angleRadians);
    static Mat4 rotateXYZ(const Vec3 & anglesRadians);
    static Mat4 Translation(const Vec3 & translation);
    static Mat4 Scale(const Vec3 & scale);
    static Mat4 Scale(float scaleX, float scaleY, float scaleZ);
    static Mat4 Scale(float scaleFactor);

    Mat4 rotate(const Vec3 & axis, float angle);
    void setIdentity();

    Mat4 operator*(const Mat4 & other) const;
    Mat4 & translate(const Vec3 & t);
    void setTranslation(const Vec3 & t);
    Mat4 removeTranslation() const;

    const float * ptr() const;
    float * ptr();

    static Mat4 orthographic(float left, float right, float bottom, float top, float near, float far);

    Vec3 getTranslation() const;
    Vec3 getScale() const;
    Vec3 getEulerAngles() const;
    Mat4 fromEulerAngles(const Vec3 & euler);

    Mat4 transpose() const;
    Mat4 inverse() const;
    Mat4 normal() const;

    friend std::ostream & operator<<(std::ostream & os, const Mat4 & m);
    float operator[](size_t index) const;
    float & operator[](size_t index);
    Mat4 & operator=(const Mat4 & other);
};

// Alias pour une transformation 3D (matrice 4x4)
// J'en avais marre de pas pouvoir dire aussi "Transform" dans le code
using Transform = Mat4;

/**
 * @brief Multiplie une matrice 4x4 par un vecteur 3D homogène (avec division par w).
 * @param mat Matrice 4x4.
 * @param vec Vecteur 3D.
 * @return Vecteur 3D transformé.
 */
inline Vec3 operator*(const Mat4 & mat, const Vec3 & vec) {
    float x = mat.data[0] * vec.x + mat.data[4] * vec.y + mat.data[8] * vec.z + mat.data[12];
    float y = mat.data[1] * vec.x + mat.data[5] * vec.y + mat.data[9] * vec.z + mat.data[13];
    float z = mat.data[2] * vec.x + mat.data[6] * vec.y + mat.data[10] * vec.z + mat.data[14];
    float w = mat.data[3] * vec.x + mat.data[7] * vec.y + mat.data[11] * vec.z + mat.data[15];

    if (w != 0.0f) {
        x /= w;
        y /= w;
        z /= w;
    }

    return Vec3(x, y, z);
}

/**
 * @brief Transforme un point 3D par une matrice sans division par w.
 * 
 * Utile pour appliquer une translation.
 * 
 * @param mat Matrice 4x4.
 * @param vec Point 3D.
 * @return Point transformé.
 */
inline Vec3 transformPoint(const Mat4 & mat, const Vec3 & vec) {
    float x = mat.data[0] * vec.x + mat.data[4] * vec.y + mat.data[8] * vec.z + mat.data[12];
    float y = mat.data[1] * vec.x + mat.data[5] * vec.y + mat.data[9] * vec.z + mat.data[13];
    float z = mat.data[2] * vec.x + mat.data[6] * vec.y + mat.data[10] * vec.z + mat.data[14];
    // PAS de division par w !
    return Vec3(x, y, z);
}

/**
 * @brief Multiplie une matrice 4x4 par un vecteur 4D.
 * @param mat Matrice 4x4.
 * @param vec Vecteur 4D.
 * @return Vecteur 4D transformé.
 */
inline Vec4 operator*(const Mat4 & mat, const Vec4 & vec) {
    Vec4 result;
    result.x = mat.data[0] * vec.x + mat.data[4] * vec.y + mat.data[8] * vec.z + mat.data[12] * vec.w;
    result.y = mat.data[1] * vec.x + mat.data[5] * vec.y + mat.data[9] * vec.z + mat.data[13] * vec.w;
    result.z = mat.data[2] * vec.x + mat.data[6] * vec.y + mat.data[10] * vec.z + mat.data[14] * vec.w;
    result.w = mat.data[3] * vec.x + mat.data[7] * vec.y + mat.data[11] * vec.z + mat.data[15] * vec.w;
    return result;
}
