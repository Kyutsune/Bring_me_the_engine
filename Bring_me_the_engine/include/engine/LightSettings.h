#pragma once
#include "math/Vec.h"
#include "math/Color.h"

/**
 * @struct LightSettings
 * @brief Contient tous les paramètres globaux d'éclairage et de brouillard pour la scène.
 * 
 * Utilisé par le LightingManager et les shaders pour définir :
 * - L'éclairage ambiant, diffus et spéculaire
 * - Les propriétés de brillance
 * - Les paramètres de brouillard (fog)
 */
struct LightSettings {
    // --- Lumière ambiante ---
    Vec3 m_ambientColor = Vec3(1.0f, 1.0f, 1.0f);  ///< Couleur de l'ambiance
    float m_ambientStrength = 0.2f;                ///< Intensité de la lumière ambiante

    // --- Lumière diffuse ---
    Vec3 m_diffuseColor = Vec3(1.0f, 1.0f, 1.0f); ///< Couleur de la lumière diffuse
    float m_diffuseIntensity = 0.5f;              ///< Intensité de la lumière diffuse

    // --- Lumière spéculaire ---
    Vec3 m_specularColor = Vec3(1.0f, 1.0f, 1.0f);///< Couleur de la lumière spéculaire
    float m_specularStrength = 0.5f;              ///< Intensité de la spéculaire
    float m_shininess = 32.0f;                    ///< Brillance du matériau

    // --- Paramètres de brouillard (fog) ---
    Color m_fogColor = Color(8, 159, 255, 255);   ///< Couleur du brouillard
    float m_fogStart = 5.0f;                      ///< Distance de début du brouillard
    float m_fogEnd = 20.0f;                       ///< Distance de fin du brouillard
    float m_fogDensity = 0.045f;                  ///< Densité du brouillard
    int m_fogType = 0;                             ///< Type de brouillard : 0=aucun, 1=linéaire, 2=exp, 3=exp²
};
