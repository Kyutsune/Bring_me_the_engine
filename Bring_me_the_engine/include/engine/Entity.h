#pragma once

#include "Material.h"
#include "Mesh.h"
#include "math/PlaneBoundingVolume.h"
#include "math/Quat.h"
#include "math/Vec.h"
#include "rendering/Shader.h"
#include <filesystem>
#include <iostream>

// TODO: foutre de l'ordre dans ce joyeux bordel

/**
 * @brief Met à jour les uniforms de caméra pour un shader donné.
 *
 * Cette fonction envoie les matrices model, view et projection au shader.
 *
 * @param shader Shader à mettre à jour
 * @param model Matrice de transformation de l'entité
 * @param view Matrice de vue de la caméra
 * @param projection Matrice de projection
 */
inline void updateCameraUniforms(Shader & shader, const Mat4 & model, const Mat4 & view, const Mat4 & projection) {
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
}

/**
 * @class Entity
 * @brief Représente un objet 3D dans la scène.
 *
 * Une entité possède un mesh, un matériau, une transformation (position, rotation, scale),
 * et peut être rendue à l'écran via un shader. Elle gère également sa bounding box pour
 * le culling et les collisions.
 */
class Entity : public std::enable_shared_from_this<Entity> {
public:
    /// Constructeur par défaut (transform identitaire, mesh nul)
    Entity() : m_transform(Mat4::identity()), m_mesh(nullptr) {}

    /**
     * @brief Constructeur avec mesh et textures
     *
     * @param transform Matrice initiale de transformation
     * @param mesh Mesh associé à l'entité
     * @param filenameTextDiffuse Chemin vers la texture diffuse
     * @param filenameNormalMap Chemin vers la normal map
     * @param filenameSpecularMap Chemin vers la specular map
     * @param name Nom de l'entité
     */
    Entity(const Mat4 & transform, std::shared_ptr<Mesh> mesh,
           const std::string & filenameTextDiffuse = "",
           const std::string & filenameNormalMap = "",
           const std::string & filenameSpecularMap = "",
           const std::string & name = "");

    /**
     * @brief Constructeur avec mesh et matériau
     *
     * @param transform Matrice initiale de transformation
     * @param mesh Mesh associé
     * @param material Matériau de l'entité
     * @param name Nom de l'entité
     */
    Entity(const Mat4 & transform, std::shared_ptr<Mesh> mesh,
           std::shared_ptr<Material> material,
           const std::string & name = "");

    ~Entity() = default;

    // --- Accesseurs ---
    const std::string & getName() const { return m_entity_name; }
    void setName(const std::string & newName) { m_entity_name = newName; }
    const Mat4 & getTransform() const { return m_transform; }
    Mat4 & getTransform() { return m_transform; }
    void setTransform(const Mat4 & newTransform);

    Material & getMaterial() { return m_material; }
    const Material & getMaterial() const { return m_material; }

    const std::shared_ptr<Mesh> & getMesh() const { return m_mesh; }

    bool isVisible() const { return visible; }
    void setVisible(bool v) { visible = v; }

    Vec3 getPosition() const { return m_position; }
    inline void setPosition(const Vec3 & pos) {
        m_transform.setTranslation(pos);
        m_position = pos;
        updateTransform();
    }

    Quat getRotation() const { return m_rotation; }
    void setRotation(const Quat & rot) {
        m_rotation = rot.normalized();
        updateTransform();
    }

    Vec3 getScale() const { return m_scale; }
    void setScale(const Vec3 & scale) {
        m_scale = scale;
        updateTransform();
    }

    Vec3 getBaseColor() const { return m_material.m_baseColor; }
    void setBaseColor(const Vec3 & color) {
        m_material.m_baseColor = color;
    }

	bool doItUseTextureDiffuse() const { return m_material.m_useDiffuse; }
    bool hasTextureDiffuse() const { return m_material.m_diffuseTexture != nullptr; }

	bool doItUseNormalMap() const { return m_material.m_useNormal; }
	bool hasNormalMap() const { return m_material.m_normalMap != nullptr; }
    
	bool doItUseSpecularMap() const { return m_material.m_useSpecular; }
	bool hasSpecularMap() const { return m_material.m_specularMap != nullptr; }

    // --- Fonctionnalités ---
    /**
     * @brief Dessine l'entité avec un shader donné
     *
     * @param shader Shader utilisé pour le rendu
     * @param view Matrice view de la caméra
     * @param projection Matrice projection de la caméra
     */
    void drawForward(Shader & shader, const Mat4 & view, const Mat4 & projection);


    void drawDeferred(Shader& shader, const Mat4& view, const Mat4& projection);

    /**
     * @brief Récupère la bounding box non transformée
     *
     * @return AABB
     */
    AABB getBoundingBox() { return m_boundingBox; }

    /**
     * @brief Récupère la bounding box transformée par la matrice de l'entité
     *
     * @return AABB
     */
    AABB getTransformedBoundingBox() const;

    /**
     * @brief Met à jour la matrice de transformation à partir de la position, rotation et scale
     */
    void updateTransform();

private:
    std::string m_entity_name;
    std::shared_ptr<Mesh> m_mesh;
    AABB m_boundingBox;

    Vec3 m_position = Vec3(0.0f);
    Quat m_rotation = Quat::identity();
    Vec3 m_scale = Vec3(1.0f, 1.0f, 1.0f);
    Mat4 m_transform;

    Material m_material;
    bool visible = false;
};
