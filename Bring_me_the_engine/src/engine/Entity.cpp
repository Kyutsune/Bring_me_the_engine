#include "engine/Entity.h"
#include "Globals.h"

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
    }
    m_entity_name = name.empty() ? "Unnamed Entity" : name;

    m_material.m_baseColor = g_selectedColor;

    setTransform(transform);
    updateTransform();
}

Entity::Entity(const Mat4 & transform, std::shared_ptr<Mesh> mesh,
               std::shared_ptr<Material> material,
               const std::string & name) : m_mesh(std::move(mesh)), m_material(std::move(*material)) {
    m_entity_name = name.empty() ? "Unnamed Entity" : name;
    if (this->m_mesh) {
        m_boundingBox = this->m_mesh->getBoundingBox();
    } else {
        std::cout << "Warning: Mesh is null for entity: " << m_entity_name << std::endl;
    }

    setTransform(transform);
    updateTransform();
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