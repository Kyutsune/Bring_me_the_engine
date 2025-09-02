#pragma once

#include "rendering/GestionTextures/Texture.h"
#include <memory>

/**
 * @struct Material
 * @brief Représente les matériaux appliqués aux entités 3D.
 * 
 * Contient les textures et flags associés :
 * - Diffuse : couleur de base de la surface
 * - Normal : détail de relief pour l'éclairage
 * - Specular : intensité et couleur des reflets spéculaires
 */
struct Material {
    /// Texture diffuse (couleur de base)
    std::shared_ptr<Texture> m_diffuseTexture = nullptr;
    
    /// Normal map (détails de relief)
    std::shared_ptr<Texture> m_normalMap = nullptr;
    
    /// Specular map (reflets spéculaires)
    std::shared_ptr<Texture> m_specularMap = nullptr;

    /// Indique si la texture diffuse est utilisée
    bool m_useDiffuse = false;

    /// Indique si la normal map est utilisée
    bool m_useNormal = false;

    /// Indique si la specular map est utilisée
    bool m_useSpecular = false;

    /// Constructeur par défaut (aucune texture)
    Material() = default;

    /**
     * @brief Constructeur avec textures
     * @param diffuse Texture diffuse
     * @param normal Normal map
     * @param specular Specular map
     */
    Material(const std::shared_ptr<Texture> & diffuse,
             const std::shared_ptr<Texture> & normal,
             const std::shared_ptr<Texture> & specular)
        : m_diffuseTexture(diffuse),
          m_normalMap(normal),
          m_specularMap(specular),
          m_useDiffuse(diffuse != nullptr),
          m_useNormal(normal != nullptr),
          m_useSpecular(specular != nullptr) {}
};
