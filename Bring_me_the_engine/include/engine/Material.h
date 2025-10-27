#pragma once

#include "rendering/GestionTextures/Texture.h"
#include "math/Vec.h"
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
    /// Couleur de base de la surface (utilisée en l'absence de texture)
    // TODO: Réfléchir à si les couleurs ne devraient pas être des std::shared_ptr<Vec3>
    Vec3 m_baseColor;

    /// On utilise une couleur par vertex ou une couleur globale
    // AVENIR: Ceci ne servira qu'au futur si on veut que le moteur garde une vision "artiste" pour dessiner précisement les mesh sommet par sommet
    // Il faudrait alors rajouter une sorte de pinceau pour dessiner proprement sur les surfaces, pour l'instant en principe ceci reste toujours à false
    bool m_useVertexColor = false;

    /// Texture diffuse
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
             const std::shared_ptr<Texture> & specular,
             const std::shared_ptr<Vec3> & basecolor = nullptr);
};
