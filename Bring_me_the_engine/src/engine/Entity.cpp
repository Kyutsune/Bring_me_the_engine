#include "engine/Entity.h"
#include "Globals.h"
#include <algorithm>

#include <map>

Entity::Entity(const Mat4 & transform, std::shared_ptr<Mesh> mesh,
               const std::string & filenameTextDiffuse,
               const std::string & filenameNormalMap,
               const std::string & filenameSpecularMap,
               const std::string & name) {


    m_entity_name = name.empty() ? "Unnamed Entity" : name;

    Material mat;
    if (!filenameTextDiffuse.empty() && std::filesystem::exists(filenameTextDiffuse)) {
        mat.m_diffuseTexture = std::make_shared<Texture>(filenameTextDiffuse);
        mat.m_useDiffuse = true;
    }
    if (!filenameNormalMap.empty() && std::filesystem::exists(filenameNormalMap)) {
		mat.m_normalMap = std::make_shared<Texture>(filenameNormalMap);
		mat.m_useNormal = true;
    }
    if(!filenameSpecularMap.empty() && std::filesystem::exists(filenameSpecularMap)) {
        mat.m_specularMap =  std::make_shared<Texture>(filenameSpecularMap);
		mat.m_useSpecular = true;
    }

    mat.m_baseColor = g_selectedColor;

    if (mesh) {
        addSubMesh(mesh, mat);
    }

    setTransform(transform);
    updateTransform();
	updatePerformanceStatsOnAddedEntity(*this);
}

Entity::Entity(const Mat4 & transform, std::shared_ptr<Mesh> mesh,
               std::shared_ptr<Material> material,
               const std::string & name) {
    m_entity_name = name.empty() ? "Unnamed Entity" : name;
    if (mesh && material) {
        addSubMesh(mesh, *material);
    }

    setTransform(transform);
    updateTransform();
	updatePerformanceStatsOnAddedEntity(*this);
}


// deprecated depuis qu'on utilise le deferred
void Entity::drawForward(Shader & shader, const Mat4 & view, const Mat4 & projection) {
    updateCameraUniforms(shader, m_transform, view, projection);


    for (auto& sub : m_subMeshes) {
        // Bind du matériau du SubMesh
        if (sub.material.m_diffuseTexture && sub.material.m_useDiffuse) {
            shader.set("useTexture", 1);
            sub.material.m_diffuseTexture->bind(0);
        }
        else {
            shader.set("useTexture", 0);
        }
        if(sub.material.m_normalMap && sub.material.m_useNormal) {
			shader.set("useNormalMap", 1);
			shader.set("texture_normal", 1);
			sub.material.m_normalMap->bind(1);
        }
        else {
			shader.set("useNormalMap", 0);
        }
		if (sub.material.m_specularMap && sub.material.m_useSpecular) {
            shader.set("useSpecularMap", 1);
            shader.set("texture_specular", 2);
            sub.material.m_specularMap->bind(2);
        }
        else {
            shader.set("useSpecularMap", 0);
        }

        shader.set("baseColor", sub.material.m_baseColor);
        shader.set("useVertexColor", sub.material.m_useVertexColor);

        for (auto& cell : sub.gridChunks) {
            cell.mesh->draw();
        }
    }   
}

void Entity::setTransform(const Mat4 & newTransform) {
    m_transform = newTransform;
    m_position = newTransform.getTranslation();
    m_rotation = Quat::fromEuler(newTransform.getEulerAngles());
    m_scale = newTransform.getScale();
}

AABB Entity::getTransformedBoundingBox() const {
    return m_boundingBox.transform(m_transform);
}

void Entity::updateTransform() {
    m_transform = Mat4::Scale(m_scale) * m_rotation.toMat4() * Mat4::Translation(m_position);
}


std::vector<SpatialCell> Entity::splitSpecificMeshIntoGrid(std::shared_ptr<Mesh> mesh, int gridRes) {
    std::vector<SpatialCell> cells;
    if (!mesh) return cells;

    const auto& sourceVertices = mesh->getVertices();
    const auto& sourceIndices = mesh->getIndices();
    AABB globalBox = mesh->getBoundingBox();

    Vec3 size = globalBox.m_max - globalBox.m_min;
    Vec3 cellSize = { size.x / gridRes, size.y / gridRes, size.z / gridRes };

    std::map<int, TempSubMeshData> gridMap;

    for (size_t i = 0; i < sourceIndices.size(); i += 3) {
        Vertex v1 = sourceVertices[sourceIndices[i]];
        Vertex v2 = sourceVertices[sourceIndices[i + 1]];
        Vertex v3 = sourceVertices[sourceIndices[i + 2]];

        Vec3 center = (v1.m_position + v2.m_position + v3.m_position) / 3.0f;

        int ix = std::clamp(int((center.x - globalBox.m_min.x) / cellSize.x), 0, gridRes - 1);
        int iy = std::clamp(int((center.y - globalBox.m_min.y) / cellSize.y), 0, gridRes - 1);
        int iz = std::clamp(int((center.z - globalBox.m_min.z) / cellSize.z), 0, gridRes - 1);
        int cellID = ix + iy * gridRes + iz * gridRes * gridRes;

        auto& data = gridMap[cellID];
        unsigned int startIdx = (unsigned int)data.vertices.size();
        data.vertices.push_back(v1);
        data.vertices.push_back(v2);
        data.vertices.push_back(v3);
        data.indices.push_back(startIdx);
        data.indices.push_back(startIdx + 1);
        data.indices.push_back(startIdx + 2);
    }

    for (auto& [id, data] : gridMap) {
        SpatialCell cell;
        cell.mesh = std::make_shared<Mesh>(data.vertices, data.indices);
        cell.localAABB = cell.mesh->getBoundingBox();
        cells.push_back(cell);
    }
    return cells;
}


void Entity::addSubMesh(std::shared_ptr<Mesh> mesh, const Material& mat) {
    SubMesh sub;
    sub.material = mat;
    sub.subMeshAABB = mesh->getBoundingBox();

    // On peut décider de découper en grille ici si le mesh est trop gros
    if (mesh->getNumberOfIndices() > 5000) {
        sub.gridChunks = splitSpecificMeshIntoGrid(mesh, 3);
    }
    else {
        // Sinon, une seule cellule qui contient tout le mesh
        SpatialCell cell;
        cell.mesh = mesh;
        cell.localAABB = sub.subMeshAABB;
        sub.gridChunks.push_back(cell);
    }

    m_subMeshes.push_back(sub);

    // Mettre à jour la bounding box globale de l'entité
    m_boundingBox.merge(sub.subMeshAABB);
}