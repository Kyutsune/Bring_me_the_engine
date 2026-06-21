#pragma once

#include "engine/Mesh.h"
#include "math/SDF/SDFVolume.h"
#include "math/Vec.h"

#include <memory>
#include <vector>

class SDFGenerator {
public:
    static std::shared_ptr<SDFVolume> getOrGenerate(const std::shared_ptr<Mesh> & mesh, const std::string & entityName, int resX = 32, int resY = 32, int resZ = 32);

private:
    // Fonction mathématique interne : distance d'un point à un triangle
    static float pointTriangleDistanceSq(const Vec3 & p, const Vec3 & a, const Vec3 & b, const Vec3 & c, Vec3 & outClosestPoint);

    static std::vector<float> computeSDFGrid(const std::shared_ptr<Mesh> & mesh, int resX, int resY, int resZ, Vec3 & outBoxMin, Vec3 & outBoxMax);

    static bool saveToCache(const std::string & cachePath, const std::vector<float> & data, int rx, int ry, int rz, const Vec3 & boxMin, const Vec3 & boxMax);
    static bool loadFromCache(const std::string & cachePath, std::vector<float> & outData, int & outRx, int & outRy, int & outZ, Vec3 & outBoxMin, Vec3 & outBoxMax);
};