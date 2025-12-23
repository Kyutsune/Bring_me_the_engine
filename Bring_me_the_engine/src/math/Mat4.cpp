/// La dedans on devrait normalement mettre les implémentations des fonctions de Vec2, Vec3 et Vec4
/// En plus de celles de Mat4 mais ses classes étant assez succintes, on peut se permettre de ne laisser
/// Ici que le coeur intéréssant pour le projet, c'est à dire les implémentations de Mat4

#include "math/Vec.h"
#include <cstring>
#include <iostream>

Mat4::Mat4() {
    for (int i = 0; i < 16; ++i)
        data[i] = 0.0f;
    data[0] = data[5] = data[10] = data[15] = 1.0f;
}

Mat4::Mat4(std::initializer_list<float> list) {
    int i = 0;
    for (auto it = list.begin(); it != list.end() && i < 16; ++it, ++i)
        data[i] = *it;
    for (; i < 16; ++i)
        data[i] = 0.0f;
}

Mat4 Mat4::identity() {
    return Mat4();
}

Mat4 Mat4::lookAt(const Vec3 & eye, const Vec3 & center, const Vec3 & up) {
    Vec3 f = (center - eye).normalized();
    Vec3 s = f.cross(up).normalized();
    Vec3 u = s.cross(f);

    Mat4 result;
    result.data[0] = s.x;
    result.data[1] = u.x;
    result.data[2] = -f.x;
    result.data[3] = 0.0f;
    result.data[4] = s.y;
    result.data[5] = u.y;
    result.data[6] = -f.y;
    result.data[7] = 0.0f;
    result.data[8] = s.z;
    result.data[9] = u.z;
    result.data[10] = -f.z;
    result.data[11] = 0.0f;
    result.data[12] = -s.dot(eye);
    result.data[13] = -u.dot(eye);
    result.data[14] = f.dot(eye);
    result.data[15] = 1.0f;

    return result;
}

Mat4 Mat4::perspective(float fovRadians, float aspect, float near, float far) {
    Mat4 result;

    memset(result.data, 0, sizeof(result.data));

    float tanHalfFovy = tan(fovRadians / 2.0f);

    result.data[0] = 1.0f / (aspect * tanHalfFovy);        // [0,0]
    result.data[5] = 1.0f / tanHalfFovy;                   // [1,1]
    result.data[10] = -(far + near) / (far - near);        // [2,2]
    result.data[11] = -1.0f;                               // [3,2] (w = -z)
    result.data[14] = -(2.0f * far * near) / (far - near); // [2,3]
    result.data[15] = 0.0f;                                // [3,3]

    return result;
}

Mat4 Mat4::rotateX(float angleRadians) {
    Mat4 result;
    float c = cos(angleRadians);
    float s = sin(angleRadians);

    result.data[5] = c;
    result.data[6] = -s;
    result.data[9] = s;
    result.data[10] = c;
    result.data[0] = 1.0f;
    result.data[15] = 1.0f;

    return result;
}

Mat4 Mat4::rotateY(float angleRadians) {
    Mat4 result;
    float c = cos(angleRadians);
    float s = sin(angleRadians);

    result.data[0] = c;
    result.data[2] = -s;
    result.data[5] = 1.0f;
    result.data[8] = s;
    result.data[10] = c;
    result.data[15] = 1.0f;

    return result;
}

Mat4 Mat4::rotateZ(float angleRadians) {
    Mat4 result;
    float c = cos(angleRadians);
    float s = sin(angleRadians);

    result.data[0] = c;
    result.data[1] = -s;
    result.data[4] = s;
    result.data[5] = c;
    result.data[10] = 1.0f;
    result.data[15] = 1.0f;

    return result;
}

Mat4 Mat4::rotateXYZ(const Vec3 & anglesRadians) {
    return Mat4::rotateZ(anglesRadians.x) *
           Mat4::rotateY(anglesRadians.y) *
           Mat4::rotateX(anglesRadians.z);
}

Mat4 Mat4::Translation(const Vec3 & t) {
    Mat4 result;
    result.data[12] = t.x;
    result.data[13] = t.y;
    result.data[14] = t.z;
    return result;
}

Mat4 Mat4::Scale(float scaleX, float scaleY, float scaleZ) {
    Mat4 result;
    result.data[0] = scaleX;
    result.data[5] = scaleY;
    result.data[10] = scaleZ;
    result.data[15] = 1.0f;
    return result;
}

Mat4 Mat4::Scale(const Vec3 & scale) {
    return Mat4::Scale(scale.x, scale.y, scale.z);
}

Mat4 Mat4::Scale(float scaleFactor) {
    return Mat4::Scale(scaleFactor, scaleFactor, scaleFactor);
}

Mat4 Mat4::rotate(const Vec3 & axis, float angle) {
    Vec3 u = axis.normalized();
    float c = cos(angle);
    float s = sin(angle);
    float one_c = 1.0f - c;

    float ux = u.x, uy = u.y, uz = u.z;

    return Mat4({c + ux * ux * one_c, ux * uy * one_c - uz * s, ux * uz * one_c + uy * s, 0.f,
                 uy * ux * one_c + uz * s, c + uy * uy * one_c, uy * uz * one_c - ux * s, 0.f,
                 uz * ux * one_c - uy * s, uz * uy * one_c + ux * s, c + uz * uz * one_c, 0.f,
                 0.f, 0.f, 0.f, 1.f});
}

void Mat4::setIdentity() {
    for (int i = 0; i < 16; ++i)
        data[i] = (i % 5 == 0) ? 1.0f : 0.0f;
}

Mat4 Mat4::operator*(const Mat4 & other) const {
    Mat4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.data[i * 4 + j] =
                data[i * 4 + 0] * other.data[0 * 4 + j] +
                data[i * 4 + 1] * other.data[1 * 4 + j] +
                data[i * 4 + 2] * other.data[2 * 4 + j] +
                data[i * 4 + 3] * other.data[3 * 4 + j];
        }
    }
    return result;
}

Mat4 & Mat4::translate(const Vec3 & t) {
    *this = *this * Mat4::Translation(t);
    return *this;
}

void Mat4::setTranslation(const Vec3 & t) {
    data[12] = t.x;
    data[13] = t.y;
    data[14] = t.z;
}

const float * Mat4::ptr() const { return data; }
float * Mat4::ptr() { return data; }

std::ostream & operator<<(std::ostream & os, const Mat4 & m) {
    return os << m.data[0] << ", " << m.data[1] << ", " << m.data[2] << ", " << m.data[3] << "\n"
              << m.data[4] << ", " << m.data[5] << ", " << m.data[6] << ", " << m.data[7] << "\n"
              << m.data[8] << ", " << m.data[9] << ", " << m.data[10] << ", " << m.data[11] << "\n"
              << m.data[12] << ", " << m.data[13] << ", " << m.data[14] << ", " << m.data[15] << "\n";
}

Mat4 Mat4::removeTranslation() const {
    Mat4 result = *this;
    result.data[12] = 0.0f;
    result.data[13] = 0.0f;
    result.data[14] = 0.0f;
    return result;
}

Mat4 Mat4::orthographic(float left, float right, float bottom, float top, float near, float far) {
    Mat4 result = Mat4::identity();

    result.data[0] = 2.0f / (right - left);
    result.data[5] = 2.0f / (top - bottom);
    result.data[10] = -2.0f / (far - near);
    result.data[12] = -(right + left) / (right - left);
    result.data[13] = -(top + bottom) / (top - bottom);
    result.data[14] = -(far + near) / (far - near);

    return result;
}

Vec3 Mat4::getTranslation() const { return Vec3(data[12], data[13], data[14]); }
Vec3 Mat4::getScale() const {
    float sx = sqrt(data[0] * data[0] + data[4] * data[4] + data[8] * data[8]);
    float sy = sqrt(data[1] * data[1] + data[5] * data[5] + data[9] * data[9]);
    float sz = sqrt(data[2] * data[2] + data[6] * data[6] + data[10] * data[10]);

    return Vec3(sx, sy, sz);
}

Vec3 Mat4::getEulerAngles() const {
    float m00 = data[0], m01 = data[1], m02 = data[2];
    float m10 = data[4], m11 = data[5], m12 = data[6];
    float m20 = data[8], m21 = data[9], m22 = data[10];

    float sy = sqrt(m00 * m00 + m01 * m01);
    bool singular = sy < 1e-6f;

    float x, y, z;

    if (!singular) {
        x = atan2(-m12, m22);
        y = atan2(-m02, sy);
        z = atan2(-m01, m11);
    } else {
        // Gestion du Gimbal Lock
        x = atan2(-m12, m22);                 // Gardez X
        y = (m02 < 0) ? M_PI / 2 : -M_PI / 2; // Y fixé à ±90°
        z = atan2(-m01, m11);                 // Recalculez Z
    }

    return Vec3(x, y, z);
}

Mat4 Mat4::fromEulerAngles(const Vec3 & euler) {
    Mat4 Rx = Mat4::rotateX(euler.x);
    Mat4 Ry = Mat4::rotateY(euler.y);
    Mat4 Rz = Mat4::rotateZ(euler.z);
    return Rz * Rx * Ry;
}

Mat4 Mat4::transpose() const {
    Mat4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.data[i * 4 + j] = data[j * 4 + i];
        }
    }
    return result;
}

Mat4 Mat4::inverse() const {
    Mat4 inv;
    const float * m = data;

    inv.data[0] = m[5] * m[10] * m[15] -
                  m[5] * m[11] * m[14] -
                  m[9] * m[6] * m[15] +
                  m[9] * m[7] * m[14] +
                  m[13] * m[6] * m[11] -
                  m[13] * m[7] * m[10];

    inv.data[4] = -m[4] * m[10] * m[15] +
                  m[4] * m[11] * m[14] +
                  m[8] * m[6] * m[15] -
                  m[8] * m[7] * m[14] -
                  m[12] * m[6] * m[11] +
                  m[12] * m[7] * m[10];

    inv.data[8] = m[4] * m[9] * m[15] -
                  m[4] * m[11] * m[13] -
                  m[8] * m[5] * m[15] +
                  m[8] * m[7] * m[13] +
                  m[12] * m[5] * m[11] -
                  m[12] * m[7] * m[9];

    inv.data[12] = -m[4] * m[9] * m[14] +
                   m[4] * m[10] * m[13] +
                   m[8] * m[5] * m[14] -
                   m[8] * m[6] * m[13] -
                   m[12] * m[5] * m[10] +
                   m[12] * m[6] * m[9];

    inv.data[1] = -m[1] * m[10] * m[15] +
                  m[1] * m[11] * m[14] +
                  m[9] * m[2] * m[15] -
                  m[9] * m[3] * m[14] -
                  m[13] * m[2] * m[11] +
                  m[13] * m[3] * m[10];

    inv.data[5] = m[0] * m[10] * m[15] -
                  m[0] * m[11] * m[14] -
                  m[8] * m[2] * m[15] +
                  m[8] * m[3] * m[14] +
                  m[12] * m[2] * m[11] -
                  m[12] * m[3] * m[10];

    inv.data[9] = -m[0] * m[9] * m[15] +
                  m[0] * m[11] * m[13] +
                  m[8] * m[1] * m[15] -
                  m[8] * m[3] * m[13] -
                  m[12] * m[1] * m[11] +
                  m[12] * m[3] * m[9];

    inv.data[13] = m[0] * m[9] * m[14] -
                   m[0] * m[10] * m[13] -
                   m[8] * m[1] * m[14] +
                   m[8] * m[2] * m[13] +
                   m[12] * m[1] * m[10] -
                   m[12] * m[2] * m[9];

    inv.data[2] = m[1] * m[6] * m[15] -
                  m[1] * m[7] * m[14] -
                  m[5] * m[2] * m[15] +
                  m[5] * m[3] * m[14] +
                  m[13] * m[2] * m[7] -
                  m[13] * m[3] * m[6];

    inv.data[6] = -m[0] * m[6] * m[15] +
                  m[0] * m[7] * m[14] +
                  m[4] * m[2] * m[15] -
                  m[4] * m[3] * m[14] -
                  m[12] * m[2] * m[7] +
                  m[12] * m[3] * m[6];

    inv.data[10] = m[0] * m[5] * m[15] -
                   m[0] * m[7] * m[13] -
                   m[4] * m[1] * m[15] +
                   m[4] * m[3] * m[13] +
                   m[12] * m[1] * m[7] -
                   m[12] * m[3] * m[5];

    inv.data[14] = -m[0] * m[5] * m[14] +
                   m[0] * m[6] * m[13] +
                   m[4] * m[1] * m[14] -
                   m[4] * m[2] * m[13] -
                   m[12] * m[1] * m[6] +
                   m[12] * m[2] * m[5];

    inv.data[3] = -m[1] * m[6] * m[11] +
                  m[1] * m[7] * m[10] +
                  m[5] * m[2] * m[11] -
                  m[5] * m[3] * m[10] -
                  m[9] * m[2] * m[7] +
                  m[9] * m[3] * m[6];

    inv.data[7] = m[0] * m[6] * m[11] -
                  m[0] * m[7] * m[10] -
                  m[4] * m[2] * m[11] +
                  m[4] * m[3] * m[10] +
                  m[8] * m[2] * m[7] -
                  m[8] * m[3] * m[6];

    inv.data[11] = -m[0] * m[5] * m[11] +
                   m[0] * m[7] * m[9] +
                   m[4] * m[1] * m[11] -
                   m[4] * m[3] * m[9] -
                   m[8] * m[1] * m[7] +
                   m[8] * m[3] * m[5];

    inv.data[15] = m[0] * m[5] * m[10] -
                   m[0] * m[6] * m[9] -
                   m[4] * m[1] * m[10] +
                   m[4] * m[2] * m[9] +
                   m[8] * m[1] * m[6] -
                   m[8] * m[2] * m[5];

    float det = m[0] * inv.data[0] + m[1] * inv.data[4] + m[2] * inv.data[8] + m[3] * inv.data[12];

    if (det == 0) {
        // Matrice non inversible, retourner une matrice identité ou gérer l'erreur
        return Mat4::identity();
    }

    det = 1.0f / det;

    for (int i = 0; i < 16; i++)
        inv.data[i] = inv.data[i] * det;

    return inv;
}


Mat4 Mat4::normal() const {
    return inverse().transpose();
}


float Mat4::operator[](size_t index) const {
    if (index >= 16) {
        throw std::out_of_range("Mat4 index out of range");
    }
    return data[index];
}

float & Mat4::operator[](size_t index) {
    if (index >= 16) {
        throw std::out_of_range("Mat4 index out of range");
    }
    return data[index];
}

Mat4 & Mat4::operator=(const Mat4 & other) {
    if (this != &other) {
        for (size_t i = 0; i < 16; ++i) {
            data[i] = other.data[i];
        }
    }
    return *this;
}