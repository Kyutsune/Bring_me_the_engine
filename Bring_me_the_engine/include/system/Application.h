#pragma once

#include <memory>
#include <vector>

struct GLFWwindow;
#include "rendering/Shader.h"
#include "ui/Menu.h"

/**
 * @brief Classe principale de l'application.
 * 
 * Cette classe gère la boucle principale, l'initialisation de GLFW, Glad, la fenêtre,
 * les callbacks, les shaders et la scène. Elle centralise aussi la gestion du menu UI.
 */
class Application {
public:
    /**
     * @brief Lance l'application.
     * 
     * Initialise toutes les ressources, entre dans la boucle principale et gère
     * la destruction des ressources à la fin.
     */
    void run();

private:
    /**
     * @brief Initialise la bibliothèque GLFW.
     * @return true si l'initialisation a réussi, false sinon.
     */
    bool initGLFW();

    /**
     * @brief Crée et configure la fenêtre OpenGL.
     * @return true si la création a réussi, false sinon.
     */
    bool initWindow();

    /**
     * @brief Initialise Glad pour charger les fonctions OpenGL.
     * @return true si l'initialisation a réussi, false sinon.
     */
    bool initGlad();

    /**
     * @brief Configure les callbacks pour GLFW (clavier, souris, redimensionnement, etc.).
     */
    void setupCallbacks();

    /**
     * @brief Initialise la scène 3D et les entités.
     */
    void setupScene();

    /**
     * @brief Charge et compile les shaders utilisés par l'application.
     */
    void setupShaders();

    /**
     * @brief Libère toutes les ressources allouées.
     */
    void cleanup();

private:
    GLFWwindow* m_window = nullptr;                     ///< Pointeur vers la fenêtre GLFW.
    std::unique_ptr<Menu> m_menu;                       ///< Gestionnaire du menu UI.
    std::vector<std::unique_ptr<Shader>> m_shaders;     ///< Liste des shaders utilisés.
};
