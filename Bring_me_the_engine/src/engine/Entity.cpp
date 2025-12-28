#include "engine/Entity.h"
#include "Globals.h"

#include <map>

Entity::Entity(const Mat4 & transform, std::shared_ptr<Mesh> mesh,
               const std::string & filenameTextDiffuse,
               const std::string & filenameNormalMap,
               const std::string & filenameSpecularMap,
               const std::string & name)
    : m_mesh(std::move(mesh)) {

    if (!filenameTextDiffuse.empty() && !std::filesystem::exists(filenameTextDiffuse)) {
        std::cout << "Erreur lors du chargement de la texture : " << filenameTextDiffuse << " La couleur seule sera utilisée" << std::endl;
    } else if (!filenameTextDiffuse.empty()) {
        m_material.m_diffuseTexture = std::make_shared<Texture>(filenameTextDiffuse);
        m_material.m_useDiffuse = true;
    }

    if (!filenameNormalMap.empty() && !std::filesystem::exists(filenameNormalMap)) {
        std::cout << "Erreur lors du chargement de la normal map : " << filenameNormalMap << " La normal map ne sera pas utilisée" << std::endl;
    } else if (!filenameNormalMap.empty()) {
        m_material.m_normalMap = std::make_shared<Texture>(filenameNormalMap);
        m_material.m_useNormal = true;
    }

    if (!filenameSpecularMap.empty() && !std::filesystem::exists(filenameSpecularMap)) {
        std::cout << "Erreur lors du chargement de la specular map : " << filenameSpecularMap << " La specular map ne sera pas utilisée" << std::endl;
    } else if (!filenameSpecularMap.empty()) {
        m_material.m_specularMap = std::make_shared<Texture>(filenameSpecularMap);
        m_material.m_useSpecular = true;
    }

    if (this->m_mesh) {
        m_boundingBox = this->m_mesh->getBoundingBox();
        if (this->m_mesh->getNumberOfIndices() > 5000) { // environ 1666 triangles
            splitMeshIntoGrid(3); // 3x3x3 = 27 morceaux max
        }
    }
    m_entity_name = name.empty() ? "Unnamed Entity" : name;

    m_material.m_baseColor = g_selectedColor;

    setTransform(transform);
    updateTransform();
	updatePerformanceStatsOnAddedEntity(*this);
}

Entity::Entity(const Mat4 & transform, std::shared_ptr<Mesh> mesh,
               std::shared_ptr<Material> material,
               const std::string & name) : m_mesh(std::move(mesh)) {
    m_entity_name = name.empty() ? "Unnamed Entity" : name;
    if (this->m_mesh) {
        m_boundingBox = this->m_mesh->getBoundingBox();
		if (this->m_mesh->getNumberOfIndices() > 5000) { // environ 1666 triangles
            splitMeshIntoGrid(3); // 3x3x3 = 27 morceaux max
        }
    }
    if(material != nullptr) {
        m_material = *material;
    } 

    setTransform(transform);
    updateTransform();
	updatePerformanceStatsOnAddedEntity(*this);
}

void Entity::drawForward(Shader & shader, const Mat4 & view, const Mat4 & projection) {
    updateCameraUniforms(shader, m_transform, view, projection);

    if (m_material.m_diffuseTexture && m_material.m_useDiffuse) {
        shader.set("useTexture", 1);
        shader.set("texture_diffuse", 0);
        m_material.m_diffuseTexture->bind();
    } else {
        shader.set("useTexture", 0);
    }

    if (m_material.m_normalMap && m_material.m_useNormal) {
        shader.set("useNormalMap", 1);
        shader.set("texture_normal", 1);
        m_material.m_normalMap->bind(1);
    } else {
        shader.set("useNormalMap", 0);
    }

    if (m_material.m_specularMap && m_material.m_useSpecular) {
        shader.set("useSpecularMap", 1);
        shader.set("texture_specular", 2);
        m_material.m_specularMap->bind(2);
    } else {
        shader.set("useSpecularMap", 0);
    }
    shader.set("baseColor", m_material.m_baseColor);
    shader.set("useVertexColor", m_material.m_useVertexColor);

    // if (this->m_entity_name == "Cube_tout_bleu") {
    //     // Mettre tous les vertices à la couleur de base
    //     Vec3 baseColor(0.8f, 0.4f, 0.4f); // Rose/rouge
    //     for (auto & v : m_mesh->getVerticesRef()) {
    //         v.m_color = baseColor;
    //     }

    //     // Mettre UN coin en bleu
    //     const Vec3 targetPos = m_mesh->getVertexRef(1).m_position;
    //     for (auto & v : m_mesh->getVerticesRef()) {
    //         if ((v.m_position - targetPos).length() < 0.0001f) {
    //             v.m_color = Vec3(0, 0, 1);
    //         }
    //     }

    //     m_mesh->upload();
    // }

    m_mesh->draw();
}

void Entity::drawDeferred(Shader& shader, const Mat4& view, const Mat4& projection) {

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



void Entity::splitMeshIntoGrid(int gridRes) {
    if (!m_mesh) return;

    const auto& sourceVertices = m_mesh->getVertices();
    const auto& sourceIndices = m_mesh->getIndices();
    AABB globalBox = m_mesh->getBoundingBox();

    Vec3 size = globalBox.m_max - globalBox.m_min;
    Vec3 cellSize = { size.x / gridRes, size.y / gridRes, size.z / gridRes };

    // Une map pour stocker les triangles par cellule (x,y,z indexés par un int unique)
    std::map<int, TempSubMeshData> gridMap;

    for (size_t i = 0; i < sourceIndices.size(); i += 3) {
        // On récupère les 3 sommets du triangle
        Vertex v1 = sourceVertices[sourceIndices[i]];
        Vertex v2 = sourceVertices[sourceIndices[i + 1]];
        Vertex v3 = sourceVertices[sourceIndices[i + 2]];

        // On calcule le centre du triangle pour savoir dans quelle cellule il va
        Vec3 center = (v1.m_position + v2.m_position + v3.m_position) / 3.0f;

        // Calcul des indices de la cellule
        int ix = std::clamp(int((center.x - globalBox.m_min.x) / cellSize.x), 0, gridRes - 1);
        int iy = std::clamp(int((center.y - globalBox.m_min.y) / cellSize.y), 0, gridRes - 1);
        int iz = std::clamp(int((center.z - globalBox.m_min.z) / cellSize.z), 0, gridRes - 1);
        int cellID = ix + iy * gridRes + iz * gridRes * gridRes;

        // Ajout des données à la cellule correspondante
        auto& data = gridMap[cellID];
        unsigned int startIdx = data.vertices.size();
        data.vertices.push_back(v1);
        data.vertices.push_back(v2);
        data.vertices.push_back(v3);
        data.indices.push_back(startIdx);
        data.indices.push_back(startIdx + 1);
        data.indices.push_back(startIdx + 2);
    }

    // On crée les vrais SubMeshes à partir de la map
    m_subMeshes.clear();
    for (auto& [id, data] : gridMap) {
        SubMesh sub;
        sub.mesh = std::make_shared<Mesh>(data.vertices, data.indices);
        sub.localAABB = sub.mesh->getBoundingBox();
        m_subMeshes.push_back(sub);
    }
}