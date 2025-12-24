#pragma once

#include "camera/Camera.h"
#include "engine/Entity.h"
#include "engine/LightingManager.h"
#include "engine/Scene.h"
#include "rendering/Shader.h"
#include "rendering/Skybox.h"
#include "rendering/ShadowManager.h"
#include "system/PathResolver.h"
#include "rendering/GpuTimer.h"
#include "rendering/GBuffer.h"

enum class RenderType {
    FORWARD,
	DEFERRED
};


/**
 * @brief Classe principale pour gérer le rendu de la scène.
 *
 * Contient les shaders pour les entités, lumières, skybox, bounding boxes et ombres.
 * Gère également le rendu des shadow maps et des éléments de debug.
 */
class Renderer {
public:
    /**
     * @brief Constructeur.
     *
     * @param entityShader Shader utilisé pour les entités 3D.
     * @param lightShader Shader utilisé pour représenter les sources lumineuses.
     * @param skyboxShader Shader pour la skybox.
     * @param boundingBoxShader Shader pour le rendu des bounding boxes.
     * @param shadowDirShader Shader pour les ombres directionnelles.
     * @param shadowPoncShader Shader pour les ombres ponctuelles.
     */
    Renderer(Shader* entityShader, Shader* lightShader, Shader* skyboxShader,
        Shader* boundingBoxShader, Shader* shadowDirShader, Shader* shadowPoncShader,
        Shader* gBufferShader, Shader* deferredLighting);

    ~Renderer();

    /**
     * @brief Rendu complet de la scène en rendu direct.
     *
     * @param scene Scène à rendre.
     */
    void renderSceneForward(const Scene & scene);


    /**
     * @brief Rendu complet de la scène en rendu différé.
     *
     * @param scene Scène à rendre.
	 */
	void renderSceneDeferred(const Scene& scene);

    /**
     * @brief Rendu de la skybox.
     *
     * @param skybox Skybox à rendre.
     * @param view Matrice view de la caméra.
     * @param projection Matrice projection de la caméra.
     */
    void renderSkybox(const Skybox * skybox, const Mat4 & view, const Mat4 & projection);

    /**
     * @brief Rendu des entités de la scène.
     *
     * @param scene Scène contenant les entités.
     * @param view Matrice view de la caméra.
     * @param projection Matrice projection de la caméra.
     */
    void renderEntities(const Scene & scene, const Mat4 & view, const Mat4 & projection);

    /**
     * @brief Rendu des entités représentant les lumières.
     *
     * @param scene Scène contenant les lumières.
     * @param view Matrice view de la caméra.
     * @param projection Matrice projection de la caméra.
     */
    void renderLightEntities(const Scene & scene, const Mat4 & view, const Mat4 & projection);

    /**
     * @brief Initialisation de la shadow map pour les lumières directionnelles.
     *
     * Méthode utile pour le debug, à utiliser avant le rendu si besoin.
     */
    inline void initShadowMap() { m_shadowManager.init_directionnal_shadows(); }

    /**
     * @brief Sauvegarde la shadow map dans un fichier image pour debug.
     *
     * @param filename Chemin du fichier de sortie.
     */
    void debugSaveShadowMap(const std::string & filename);

    /**
     * @brief Rend la shadow map sur un quad pour visualisation/debug.
     */
    void renderShadowMapOnQuad();

    /**
     * @brief Rendu complet d'une frame, incluant entités, lumières et skybox.
     *
     * @param scene Scène à rendre.
     */
    void renderFrame(const Scene & scene);






	// Partie de debug GBuffer
    GLuint fullscreenVAO;


private:
    Shader * m_entityShader;             ///< Shader pour les entités.
    Shader * m_lightShader;              ///< Shader pour les lumières.
    Shader * m_skyboxShader;             ///< Shader pour la skybox.
    Shader * m_boundingBoxShader;        ///< Shader pour les bounding boxes.
    Shader * m_shadowShaderDirectionnal; ///< Shader pour les ombres directionnelles.
    Shader * m_shadowShaderPonctual;     ///< Shader pour les ombres ponctuelles.
	Shader * m_gBufferShader;             ///< Shader pour le G-Buffer (rendu différé).
	Shader * m_deferredLightingShader;    ///< Shader pour la passe d'éclairage en rendu différé.

    ShadowManager m_shadowManager; ///< Gestionnaire des shadow maps.

    GpuTimer m_mainRenderTimer;   ///< Timer GPU pour mesurer le temps de rendu principal.
    GpuTimer m_shadowRenderTimer; ///< Timer GPU pour mesurer le temps de rendu des ombres.
    GpuTimer m_sceneRenderTimer;  ///< Timer GPU pour mesurer le temps de rendu de la scène.


	GBuffer m_gBuffer;           ///< G-Buffer pour le rendu différé.


    RenderType m_renderType;


    // AVENIR: utile seulement pour debug, pourrait être déplacé
    Shader m_quadDebugShader = Shader(
        PathResolver::getResourcePath("shaders/debug/quad.vert"),
        PathResolver::getResourcePath("shaders/debug/quad.frag"));
};
