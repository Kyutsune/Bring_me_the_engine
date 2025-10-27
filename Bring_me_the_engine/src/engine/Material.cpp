#include "engine/Material.h"
#include "Globals.h"
Material::Material(const std::shared_ptr<Texture> & diffuse,
                   const std::shared_ptr<Texture> & normal,
                   const std::shared_ptr<Texture> & specular,
                   const std::shared_ptr<Vec3> & basecolor)
    : m_diffuseTexture(diffuse),
      m_normalMap(normal),
      m_specularMap(specular),
      m_useDiffuse(diffuse != nullptr),
      m_useNormal(normal != nullptr),
      m_useSpecular(specular != nullptr) {
    if (basecolor == nullptr) {
        m_baseColor = g_selectedColor;
    } else {
        m_baseColor = *basecolor;
    }
}