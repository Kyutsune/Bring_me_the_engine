#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <array>
#include "engine/Mesh.h"
#include "math/Tang_Bitang.h"

namespace MeshLoaderOBJ {
    static std::shared_ptr<Mesh> loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "[OBJ Loader ERROR] Impossible d'ouvrir " << filepath << std::endl;
            return std::make_shared<Mesh>(std::vector<Vertex>{}, std::vector<unsigned int>{});
        }

        std::vector<Vec3> positions;
        std::vector<Vec3> normals;
        std::vector<Vec2> texCoords;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            // Nettoyage des fins de ligne Windows
            if (!line.empty() && line.back() == '\r') line.pop_back();

            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "v") {
                Vec3 pos;
                ss >> pos.x >> pos.y >> pos.z;
                positions.push_back(pos);
            }
            else if (prefix == "vt") {
                Vec2 uv;
                ss >> uv.x >> uv.y;
                texCoords.push_back(uv);
            }
            else if (prefix == "vn") {
                Vec3 n;
                ss >> n.x >> n.y >> n.z;
                normals.push_back(n);
            }
            else if (prefix == "f") {
                std::vector<std::string> faceVertices;
                std::string vStr;
                while (ss >> vStr) faceVertices.push_back(vStr);

                // Triangulation des faces (gère les quads de house.obj)
                for (size_t i = 1; i + 1 < faceVertices.size(); ++i) {
                    std::array<std::string, 3> tri = { faceVertices[0], faceVertices[i], faceVertices[i + 1] };
                    for (auto& s : tri) {
                        // Remplacement propre des // pour ne pas décaler les indices
                        size_t pos_slash;
                        while ((pos_slash = s.find("//")) != std::string::npos) s.replace(pos_slash, 2, "/0/");
                        std::replace(s.begin(), s.end(), '/', ' ');

                        std::stringstream vs(s);
                        int vIdx = 0, tIdx = 0, nIdx = 0;
                        vs >> vIdx >> tIdx >> nIdx;

                        // Gestion des indices relatifs (négatifs) ou absolus (positifs)
                        int finalV = (vIdx > 0) ? vIdx - 1 : (vIdx < 0 ? (int)positions.size() + vIdx : -1);
                        int finalT = (tIdx > 0) ? tIdx - 1 : (tIdx < 0 ? (int)texCoords.size() + tIdx : -1);
                        int finalN = (nIdx > 0) ? nIdx - 1 : (nIdx < 0 ? (int)normals.size() + nIdx : -1);

                        if (finalV >= 0 && finalV < (int)positions.size()) {
                            Vertex v;
                            v.m_position = positions[finalV];

                            // Si finalT est -1 ou hors limites, on met (0,0)
                            v.m_texCoords = (finalT >= 0 && finalT < (int)texCoords.size())
                                ? texCoords[finalT] : Vec2(0.0f, 0.0f);

                            v.m_normal = (finalN >= 0 && finalN < (int)normals.size())
                                ? normals[finalN] : Vec3(0, 0, 0);

                            v.m_color = Vec3(1.0f);
                            vertices.push_back(v);
                            indices.push_back((unsigned int)vertices.size() - 1);
                        }
                    }
                }
            }
        }

        std::cout << "[OBJ Loader] " << filepath << " : " << positions.size() << " pos, "
            << texCoords.size() << " UVs, " << normals.size() << " normals, "
            << vertices.size() << " vertices, " << indices.size() << " indices." << std::endl;

        if (vertices.empty()) return std::make_shared<Mesh>(vertices, indices);

        computeTangentsAndBitangents(vertices, indices);
        return std::make_shared<Mesh>(vertices, indices);
    }
};