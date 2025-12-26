#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <map>
#include "rendering/GestionTextures/Texture.h"

//AVENIR: Dans un futur hypothétique ou Bring me the engine se développe plus, le fait de garder en cache
// Toutes les textures tout le temps peut devenir problématique en termes de mémoire.
// Il serait peut-être judicieux de mettre en place un système de gestion de mémoire plus sophistiqué,
// comme un système de chargement/déchargement dynamique des textures en fonction de leur utilisation.

struct TextureKey {
    std::string path;
    GLint wrap;
    GLint filter;
    bool flip;

    // Nécessaire pour l'utiliser comme clé dans un std::map
    bool operator<(const TextureKey& other) const {
        return std::tie(path, wrap, filter, flip) < std::tie(other.path, other.wrap, other.filter, other.flip);
    }
};


class TextureManager {
public:
    static std::shared_ptr<Texture> load(const std::string& path, GLint wrapMode = GL_REPEAT, GLint filterMode = GL_NEAREST, bool flip = true);
    static void clear();
    static void clear(const std::string& path);

    static size_t getTextureCount();

private:
    static inline std::map<TextureKey, std::shared_ptr<Texture>> cache;
};