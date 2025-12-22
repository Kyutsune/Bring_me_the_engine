#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Callbacks {

    /**
     * @brief Fonction appelée lors d'un événement clavier.
     *
     * @param window Fenêtre GLFW ciblée.
     * @param key Code de la touche.
     * @param scancode Code scancode de la touche.
     * @param action Type d'action (appui, relâchement, répétition).
     * @param mods Modificateurs (Shift, Ctrl, Alt, etc.).
     */
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    /**
     * @brief Fonction appelée lors du déplacement de la souris.
     *
     * @param window Fenêtre GLFW ciblée.
     * @param xpos Position X de la souris dans la fenêtre.
     * @param ypos Position Y de la souris dans la fenêtre.
     */
    void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

}
