#pragma once

#include "engine/Scene.h"
#include "rendering/Shader.h"
#include "shadows/DirectionalShadowMap.h"
#include "shadows/PonctualShadowMap.h"

/**
 * @brief Gère les ombres dans la scène.
 * 
 * Cette classe centralise la création, le rendu et la liaison des shadow maps
 * directionnelles et ponctuelles avec les shaders principaux.
 */
class ShadowManager {
public:
    /**
     * @brief Constructeur.
     * @param dirShadowShader Shader utilisé pour les ombres directionnelles.
     * @param pointShadowShader Shader utilisé pour les ombres ponctuelles.
     */
    ShadowManager(Shader * dirShadowShader, Shader * pointShadowShader);

    /**
     * @brief Initialise la shadow map directionnelle.
     * 
     * Configure la shadow map et les buffers nécessaires pour le rendu des ombres directionnelles.
     */
    void init_directionnal_shadows();

    /**
     * @brief Rend toutes les ombres de la scène.
     * @param scene Référence à la scène à rendre.
     */
    void renderShadows(const Scene & scene);

    /**
     * @brief Envoie les informations de shadow maps aux shaders principaux.
     * 
     * @param shader Shader cible auquel envoyer les textures et matrices d'ombres.
     * @param scene Référence à la scène pour récupérer les lumières.
     */
    void bindShadows(Shader & shader, const Scene & scene);

    /**
     * @brief Sauvegarde la shadow map directionnelle dans un fichier pour debug.
     * @param filename Nom du fichier image.
     */
    void debugSaveDirectionalShadow(const std::string & filename);

    /**
     * @brief Rend un quad de debug pour visualiser les shadow maps.
     */
    void renderDebugQuad();

private:
    DirectionalShadowMap m_directionalShadow;       ///< Shadow map directionnelle.
    std::vector<PonctualShadowMap> m_punctualShadows; ///< Shadow maps ponctuelles.

    Shader * m_dirShadowShader;    ///< Shader utilisé pour les ombres directionnelles.
    Shader * m_pointShadowShader;  ///< Shader utilisé pour les ombres ponctuelles.

    bool m_punctualShadowEnabled = true; ///< Indique si les ombres ponctuelles sont actives.
};
