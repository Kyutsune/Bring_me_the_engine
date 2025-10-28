#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "engine/Mesh.h"
#include "math/Tang_Bitang.h"

namespace MeshLoaderOBJ {
    static std::shared_ptr<Mesh> loadFromFile(const std::string & filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "[OBJ Loader] Impossible d'ouvrir " << filepath << std::endl;
            return std::make_shared<Mesh>(std::vector<Vertex>{}, std::vector<unsigned int>{});
        }

        std::vector<Vec3> positions;
        std::vector<Vec3> normals;
        std::vector<Vec2> texCoords;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "v") {
                Vec3 pos;
                ss >> pos.x >> pos.y >> pos.z;
                positions.push_back(pos);
            } else if (prefix == "vt") {
                Vec2 uv;
                ss >> uv.x >> uv.y;
                uv.y = 1.0f - uv.y; // Inverser l'axe Y pour OpenGL
                texCoords.push_back(uv);
            } else if (prefix == "vn") {
                Vec3 n;
                ss >> n.x >> n.y >> n.z;
                normals.push_back(n);
            } else if (prefix == "f") {
                std::vector<std::string> faceVertices;
                std::string vertexStr;
                while (ss >> vertexStr)
                    faceVertices.push_back(vertexStr);

                for (size_t i = 1; i + 1 < faceVertices.size(); ++i) {
                    std::array<std::string, 3> tri = {faceVertices[0], faceVertices[i], faceVertices[i + 1]};
                    for (auto & vStr : tri) {
                        std::replace(vStr.begin(), vStr.end(), '/', ' ');
                        std::stringstream vs(vStr);
                        int vIdx = 0, tIdx = 0, nIdx = 0;
                        vs >> vIdx >> tIdx >> nIdx;

                        Vertex vertex;
                        vertex.m_position = positions[vIdx - 1];
                        vertex.m_texCoords = (tIdx > 0 && tIdx <= (int)texCoords.size()) ? texCoords[tIdx - 1] : Vec2(0, 0);
                        vertex.m_normal = (nIdx > 0 && nIdx <= (int)normals.size()) ? normals[nIdx - 1] : Vec3(0, 0, 0);
                        vertex.m_color = Vec3(1.0f);
                        vertices.push_back(vertex);
                        indices.push_back(vertices.size() - 1);
                    }
                }
            }
        }

        computeTangentsAndBitangents(vertices, indices);
        std::cout << "[OBJ Loader] " << filepath << " chargé avec "
                  << vertices.size() << " vertices et "
                  << indices.size() / 3 << " triangles." << std::endl;

        return std::make_shared<Mesh>(vertices, indices);
    }
};
