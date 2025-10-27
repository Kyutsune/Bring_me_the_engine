#include "math/Vec.h"

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

Vec3::Vec3(const Color & color)
    : x(color.r), y(color.g), z(color.b) {}

Vec3 Vec3::operator+(const Vec3 & other) const {
    return {x + other.x, y + other.y, z + other.z};
}

Vec3 Vec3::operator-(const Vec3 & other) const {
    return {x - other.x, y - other.y, z - other.z};
}

Vec3 Vec3::operator*(float scalar) const {
    return {x * scalar, y * scalar, z * scalar};
}

Vec3 Vec3::operator*(const Vec3 & other) const {
    return {x * other.x, y * other.y, z * other.z};
}

Vec3 Vec3::operator/(float scalar) const {
    if (scalar == 0.0f) {
        throw std::runtime_error("Division par zéro dans Vec3::operator/");
    }
    return {x / scalar, y / scalar, z / scalar};
}

bool Vec3::operator==(const Vec3 & other) const {
    float epsilon = 1e-6f;
    return std::abs(x - other.x) < epsilon &&
           std::abs(y - other.y) < epsilon &&
           std::abs(z - other.z) < epsilon;
}

bool Vec3::operator!=(const Vec3 & other) const {
    return !(*this == other);
}

Vec3 Vec3::operator+=(const Vec3 & other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vec3 Vec3::operator-=(const Vec3 & other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vec3 Vec3::operator-() const {
    return Vec3(-x, -y, -z);
}

const float * Vec3::ptr() const {
    return &x;
}

float * Vec3::ptr() {
    return &x;
}

float Vec3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::normalized() const {
    float len = length();
    return len == 0 ? *this : *this / len;
}

float Vec3::dot(const Vec3 & other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vec3 Vec3::cross(const Vec3 & other) const {
    return {
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x};
}

Vec3 operator*(float scalar, const Vec3 & v) {
    return {v.x * scalar, v.y * scalar, v.z * scalar};
}

std::ostream & operator<<(std::ostream & os, const Vec3 & v) {
    return os << "Vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
}