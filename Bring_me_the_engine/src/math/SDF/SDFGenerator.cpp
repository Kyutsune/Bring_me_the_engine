#include "math/SDF/SDFGenerator.h"
#include "system/PathResolver.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

// Algorithme géométrique standard pour trouver le point le plus proche sur un triangle
float SDFGenerator::pointTriangleDistanceSq(const Vec3 & p, const Vec3 & a, const Vec3 & b, const Vec3 & c, Vec3 & outClosestPoint) {
    Vec3 ab = b - a;
    Vec3 ac = c - a;
    Vec3 ap = p - a;

    float d1 = ab.dot(ap);
    float d2 = ac.dot(ap);
    if (d1 <= 0.0f && d2 <= 0.0f) {
        outClosestPoint = a;
        return (p - a).lengthSq();
    }

    Vec3 bp = p - b;
    float d3 = ab.dot(bp);
    float d4 = ac.dot(bp);
    if (d3 >= 0.0f && d4 <= d3) {
        outClosestPoint = b;
        return (p - b).lengthSq();
    }

    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        float v = d1 / (d1 - d3);
        outClosestPoint = a + ab * v;
        return (p - outClosestPoint).lengthSq();
    }

    Vec3 cp = p - c;
    float d5 = ab.dot(cp);
    float d6 = ac.dot(cp);
    if (d6 >= 0.0f && d5 <= d6) {
        outClosestPoint = c;
        return (p - c).lengthSq();
    }

    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float w = d2 / (d2 - d6);
        outClosestPoint = a + ac * w;
        return (p - outClosestPoint).lengthSq();
    }

    float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        outClosestPoint = b + (c - b) * w;
        return (p - outClosestPoint).lengthSq();
    }

    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    outClosestPoint = a + ab * v + ac * w;
    return (p - outClosestPoint).lengthSq();
}

std::vector<float> SDFGenerator::computeSDFGrid(const std::shared_ptr<Mesh> & mesh, int resX, int resY, int resZ, Vec3 & outBoxMin, Vec3 & outBoxMax) {
    AABB box = mesh->getBoundingBox();

    float minThickness = 0.05f;
    if ((box.m_max.y - box.m_min.y) < minThickness) {
        box.m_min.y -= minThickness * 0.5f;
        box.m_max.y += minThickness * 0.5f;
    }

    Vec3 size = box.m_max - box.m_min;

    outBoxMin = box.m_min;
    outBoxMax = box.m_max;

    std::vector<float> sdfData(resX * resY * resZ);
    const auto & vertices = mesh->getVertices();
    const auto & indices = mesh->getIndices();

    std::cout << "[SDF GENERATOR] Calcul OpenMP de la grille (" << resX << "x" << resY << "x" << resZ << ")..." << std::endl;

#pragma omp parallel for collapse(3)
    for (int x = 0; x < resX; ++x) {
        for (int y = 0; y < resY; ++y) {
            for (int z = 0; z < resZ; ++z) {
                Vec3 p(
                    box.m_min.x + (static_cast<float>(x) / (resX - 1)) * size.x,
                    box.m_min.y + (static_cast<float>(y) / (resY - 1)) * size.y,
                    box.m_min.z + (static_cast<float>(z) / (resZ - 1)) * size.z);

                float minDistanceSq = 1e30f;
                Vec3 closestPoint(0.0f);
                Vec3 closestTriangleNormal(0.0f, 1.0f, 0.0f);

                for (size_t i = 0; i < indices.size(); i += 3) {
                    Vec3 a = vertices[indices[i]].m_position;
                    Vec3 b = vertices[indices[i + 1]].m_position;
                    Vec3 c = vertices[indices[i + 2]].m_position;

                    Vec3 currentClosest;
                    float distSq = pointTriangleDistanceSq(p, a, b, c, currentClosest);

                    if (distSq < minDistanceSq) {
                        minDistanceSq = distSq;
                        closestPoint = currentClosest;
                        Vec3 ab = b - a;
                        closestTriangleNormal = ab.cross(c - a).normalized();
                    }
                }

                float distance = std::sqrt(minDistanceSq);
                Vec3 pToClosest = p - closestPoint;
                if (pToClosest.dot(closestTriangleNormal) < 0.0f) {
                    distance = -distance;
                }

                int index = x + y * resX + z * resX * resY;
                sdfData[index] = distance;
            }
        }
    }
    return sdfData;
}

std::shared_ptr<SDFVolume> SDFGenerator::getOrGenerate(const std::shared_ptr<Mesh> & mesh, const std::string & entityName, int resX, int resY, int resZ) {
    if (!mesh || mesh->getNumberOfIndices() == 0)
        return nullptr;

    // Génération d'un nom de fichier propre sans espaces
    std::string safeName = entityName;
    std::replace(safeName.begin(), safeName.end(), ' ', '_');

    // On s'assure que le dossier de cache existe à la racine des ressources
    std::string cacheDir = PathResolver::getResourcePath("assets/sdf_cache");
    if (!std::filesystem::exists(cacheDir)) {
        std::filesystem::create_directories(cacheDir);
    }

    std::string cachePath = cacheDir + "/" + safeName + ".sdf";

    std::vector<float> sdfData;
    Vec3 boxMin, boxMax;

    // Tentative de chargement depuis le cache
    if (std::filesystem::exists(cachePath) && loadFromCache(cachePath, sdfData, resX, resY, resZ, boxMin, boxMax)) {
        std::cout << "[SDF GENERATOR] Cache trouve !..." << std::endl;

        auto sdfVolume = std::make_shared<SDFVolume>();
        sdfVolume->buildTexture(sdfData, resX, resY, resZ);

        sdfVolume->setLocalBounds(boxMin, boxMax);
        return sdfVolume;
    }

    //  Pas de cache ou échec : on calcule la grille via OpenMP
    std::cout << "[SDF GENERATOR] Aucun cache valide..." << std::endl;
    sdfData = computeSDFGrid(mesh, resX, resY, resZ, boxMin, boxMax);

    if (saveToCache(cachePath, sdfData, resX, resY, resZ, boxMin, boxMax)) {
        std::cout << "[SDF GENERATOR] Grille sauvegardee..." << std::endl;
    }

    auto sdfVolume = std::make_shared<SDFVolume>();
    sdfVolume->buildTexture(sdfData, resX, resY, resZ);

    sdfVolume->setLocalBounds(boxMin, boxMax);
    return sdfVolume;
}

bool SDFGenerator::saveToCache(const std::string & cachePath, const std::vector<float> & data, int rx, int ry, int rz, const Vec3 & boxMin, const Vec3 & boxMax) {
    std::ofstream out(cachePath, std::ios::binary);
    if (!out.is_open())
        return false;

    // En-tête : Résolution
    out.write(reinterpret_cast<const char *>(&rx), sizeof(int));
    out.write(reinterpret_cast<const char *>(&ry), sizeof(int));
    out.write(reinterpret_cast<const char *>(&rz), sizeof(int));

    // En-tête : Bornes de la Bounding Box
    out.write(reinterpret_cast<const char *>(&boxMin), sizeof(Vec3));
    out.write(reinterpret_cast<const char *>(&boxMax), sizeof(Vec3));

    // Données brutes de la grille
    size_t dataSize = data.size() * sizeof(float);
    out.write(reinterpret_cast<const char *>(data.data()), dataSize);

    out.close();
    return true;
}

bool SDFGenerator::loadFromCache(const std::string & cachePath, std::vector<float> & outData, int & outRx, int & outRy, int & outRz, Vec3 & outBoxMin, Vec3 & outBoxMax) {
    std::ifstream in(cachePath, std::ios::binary);
    if (!in.is_open())
        return false;

    // Lecture de l'en-tête
    in.read(reinterpret_cast<char *>(&outRx), sizeof(int));
    in.read(reinterpret_cast<char *>(&outRy), sizeof(int));
    in.read(reinterpret_cast<char *>(&outRz), sizeof(int));
    in.read(reinterpret_cast<char *>(&outBoxMin), sizeof(Vec3));
    in.read(reinterpret_cast<char *>(&outBoxMax), sizeof(Vec3));

    // Allocation du vecteur et lecture des floats
    outData.resize(outRx * outRy * outRz);
    in.read(reinterpret_cast<char *>(outData.data()), outData.size() * sizeof(float));

    in.close();
    return true;
}