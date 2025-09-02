#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "engine/Scene.h"
#include "ui/Sections.h"
#include "Globals.h"

/**
 * @brief Classe gérant le menu graphique via ImGui.
 * 
 * Permet de créer, afficher et gérer l'interaction avec les menus principaux
 * et les sections d'édition de scène.  
 * Le menu est lié à une fenêtre GLFW et à la scène active.
 */
class Menu {
public:
    /**
     * @brief Constructeur du menu.
     * 
     * @param window Pointeur vers la fenêtre GLFW sur laquelle le menu sera rendu.
     */
    Menu(GLFWwindow * window);

    /** @brief Destructeur du menu. */
    ~Menu();

    /**
     * @brief Début d'un nouveau frame ImGui.
     * 
     * Prépare ImGui pour le rendu du menu sur la frame actuelle.
     */
    void beginFrame();

    /**
     * @brief Rendu du menu à l'écran.
     * 
     * Appelle les différentes sections du menu et les affiche si elles sont actives.
     */
    void render();

    /**
     * @brief Termine le rendu de la frame ImGui.
     * 
     * Nettoie et finalise l'affichage du menu pour cette frame.
     */
    void endFrame();

    /**
     * @brief Configuration initiale de l'affichage du menu.
     * 
     * Permet de définir les styles, tailles minimales/maximales et autres paramètres ImGui.
     */
    void setupMenuDisplay();

protected:
    GLFWwindow * window;  ///< Fenêtre GLFW sur laquelle le menu est rendu.
    Scene * scene;        ///< Pointeur vers la scène active, pour afficher ou modifier des objets.

private:
    bool m_menu_principal = true;       ///< Flag indiquant si le menu principal est visible.
    bool m_menu_scene_editor = true;    ///< Flag indiquant si le menu d'édition de scène est visible.

    int m_taille_minimale_x;  ///< Largeur minimale de la fenêtre du menu
    int m_taille_minimale_y;  ///< Hauteur minimale de la fenêtre du menu
    int m_taille_maximale_x;  ///< Largeur maximale de la fenêtre du menu
    int m_taille_maximale_y;  ///< Hauteur maximale de la fenêtre du menu
};
