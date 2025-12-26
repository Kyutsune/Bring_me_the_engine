#include "rendering/GestionTextures/TextureManager.h"

std::shared_ptr<Texture> TextureManager::load(const std::string& path, GLint wrapMode, GLint filterMode, bool flip) {
    TextureKey key = { path, wrapMode, filterMode, flip };

    auto it = cache.find(key);
    if (it != cache.end())
        return it->second;

    auto texture = std::make_shared<Texture>(path, wrapMode, filterMode, flip);
    cache[key] = texture;
    return texture;
}
void TextureManager::clear() {
    cache.clear();
}

void TextureManager::clear(const std::string& path) {
    // Comme un path peut avoir plusieurs clés (différents wraps/filters),
    // on doit itérer pour supprimer toutes les occurrences.
    for (auto it = cache.begin(); it != cache.end(); ) {
        if (it->first.path == path) {
            it = cache.erase(it);
        }
        else {
            ++it;
        }
    }
}

size_t TextureManager::getTextureCount() {
    return cache.size();
}