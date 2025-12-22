#pragma once

#include <filesystem>
#include <string>

/**
 * @brief Classe utilitaire pour résoudre les chemins d'accès aux fichiers.
 * 
 * Fournit des fonctions statiques pour obtenir le répertoire de l'exécutable
 * et pour construire des chemins vers les ressources du projet.
 */
class PathResolver {
public:
    /**
     * @brief Retourne le chemin du dossier contenant l'exécutable.
     * 
     * Utile pour charger des fichiers de configuration ou des ressources
     * relatives à l'exécutable.
     * 
     * @return std::filesystem::path Chemin vers le dossier de l'exécutable.
     */
    static std::filesystem::path getExecutableDir();

    /**
     * @brief Construit un chemin vers un fichier ressource depuis le dossier du projet.
     * 
     * Permet de résoudre facilement le chemin d'accès à des fichiers shaders,
     * textures ou autres ressources.
     * 
     * @param relative Chemin relatif à partir du dossier des ressources.
     * @return std::filesystem::path Chemin absolu vers le fichier ressource.
     */
    static const std::string getResourcePath(const std::string& relative);
};
