#pragma once
#include "math/Color.h"
#include "math/Vec.h"

/**
 * @enum LightType
 * @brief Types de lumière supportés par le moteur.
 */
enum LightType {
    LIGHT_ERROR = -1,   ///< Type invalide
    LIGHT_POINT = 0,    ///< Lumière ponctuelle
    LIGHT_DIRECTIONAL = 1, ///< Lumière directionnelle
    // AVENIR: Spotlight, Area light, etc.
};

/**
 * @class Light
 * @brief Représente une source lumineuse dans la scène.
 * 
 * Gère les propriétés de la lumière, son type, sa couleur, son intensité et
 * les paramètres d'atténuation pour les lumières ponctuelles.
 */
class Light {
public:
    /// Constructeur par défaut (lumière inactive)
    Light();

    /**
     * @brief Constructeur complet
     * @param active Indique si la lumière est active
     * @param type Type de lumière (POINT ou DIRECTIONAL)
     * @param position Position dans l'espace (pour lumière ponctuelle)
     * @param direction Direction de la lumière (pour directionnelle)
     * @param color Couleur de la lumière
     * @param intensity Intensité lumineuse
     * @param constant Coefficient constant d'atténuation (pour point)
     * @param linear Coefficient linéaire d'atténuation (pour point)
     * @param quadratic Coefficient quadratique d'atténuation (pour point)
     */
    Light(bool active, int type, const Vec3 & position, const Vec3 & direction,
          const Color & color, float intensity, float constant, float linear, float quadratic);

    /**
     * @brief Calcule la portée effective de la lumière ponctuelle.
     * 
     * Utilise les coefficients d'atténuation pour déterminer jusqu'où
     * la lumière a un effet perceptible.
     * @param epsilon Seuil minimal de contribution lumineuse
     * @return Distance maximale à laquelle la lumière est perceptible
     */
    float computeEffectiveRange(float epsilon) const;

    // --- Accesseurs ---
    LightType getType() const;
    void setType(LightType newType);

    const Vec3 & getPosition() const;
    void setPosition(const Vec3 & newPosition);

    const Vec3 & getDirection() const;

    const Color & getColor() const;
    Color & getColor();

    float getIntensity() const;
    void setIntensity(float newIntensity);

    float getConstant() const;
    float getLinear() const;
    float getQuadratic() const;

    bool isActive() const;
    void setActive(bool isActive);

private:
    bool m_active;         ///< Indique si la lumière est active
    LightType m_type;      ///< Type de lumière (0=point, 1=directionnelle)
    Vec3 m_position;       ///< Position dans l'espace (pour point)
    Vec3 m_direction;      ///< Direction de la lumière (pour directionnelle)
    Color m_color;         ///< Couleur de la lumière
    float m_intensity;     ///< Intensité lumineuse

    // Paramètres d'atténuation pour lumière ponctuelle
    float m_constant;
    float m_linear;
    float m_quadratic;
};
