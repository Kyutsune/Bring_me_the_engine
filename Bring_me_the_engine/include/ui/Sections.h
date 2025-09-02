#pragma once

#include "Globals.h"
#include "engine/Scene.h"

namespace Sections {

    /**
     * @brief Affiche la section de gestion des textures dans le menu principal.
     * 
     * Permet de visualiser ou modifier les textures des objets présents dans la scène.
     * @param scene Pointeur vers la scène active.
     */
    void textureSection(Scene * scene);

    /**
     * @brief Affiche la section de gestion des lumières globales.
     * 
     * Permet d'activer/désactiver les lumières et d'ajuster leurs paramètres globaux.
     * @param scene Pointeur vers la scène active.
     */
    void lightSection(Scene * scene);

    /**
     * @brief Section spécifique aux lumières directionnelles.
     * 
     * Permet de modifier la direction, l'intensité et la couleur des lumières directionnelles.
     * @param scene Pointeur vers la scène active.
     */
    void directionnalLightSection(Scene * scene);

    /**
     * @brief Section pour les lumières ponctuelles.
     * 
     * Permet de modifier la position, l'intensité et la couleur des lumières ponctuelles.
     * @param scene Pointeur vers la scène active.
     */
    void ponctualLightSection(Scene * scene);

    /**
     * @brief Section permettant de modifier la sensibilité des contrôles.
     */
    void sensitivitySection();

    /**
     * @brief Section pour gérer le brouillard (fog) de la scène.
     * 
     * Permet de régler la densité, la couleur et la distance du brouillard.
     * @param scene Pointeur vers la scène active.
     */
    void fogSection(Scene * scene);

    /**
     * @brief Section pour la gestion des scènes.
     * 
     * Permet de charger ou changer de scène.
     * @param scene Pointeur vers la scène active.
     * @return bool Vrai si la scène a été modifiée.
     */
    bool SceneSection(Scene * scene);

    /**
     * @brief Bouton permettant de quitter l'application depuis le menu.
     * @param window Pointeur vers la fenêtre GLFW.
     */
    void quitButton(GLFWwindow * window);

    /**
     * @brief Section pour manipuler les objets présents dans la scène.
     * 
     * Permet de sélectionner, ajouter ou supprimer des entités.
     * @param scene Pointeur vers la scène active.
     */
    void objectSection(Scene * scene);

    /**
     * @brief Section spécifique à l'édition de la scène lors de la création d'entités.
     * 
     * Permet de changer le type d'entité à créer et ses propriétés.
     * @param scene Pointeur vers la scène active.
     */
    void changeEntityCreatedSection(Scene * scene);

}
