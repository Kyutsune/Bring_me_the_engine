#include "system/Application.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include "Globals.h"
#include "input/Callbacks.h"
#include "input/ClavierSouris.h"
#include "rendering/Renderer.h"
#include "system/PathResolver.h"

#include "rendering/GestionTextures/TextureManager.h"

#include <filesystem>

void Application::run() {
    if (!initGLFW() || !initWindow() || !initGlad())
        return;

    setupCallbacks();

    glViewport(0, 0, g_windowWidth, g_windowHeight);
    glEnable(GL_DEPTH_TEST);

    setupScene();
    setupShaders();

    Renderer renderer(
        m_shaders[0].get(),
        m_shaders[1].get(),
        m_shaders[2].get(),
        m_shaders[3].get(),
        m_shaders[4].get(),
        m_shaders[5].get(),
        m_shaders[6].get(),
        m_shaders[7].get()
    );
	g_renderer = &renderer;

    while (!glfwWindowShouldClose(m_window)) {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_menu->beginFrame();
        ClavierSouris::update(m_window);
        g_scenePtr->update();
        renderer.renderFrame(*g_scenePtr);
        m_menu->render();
        m_menu->endFrame();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    cleanup();
}

bool Application::initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Erreur : initialisation de GLFW échouée\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return true;
}

bool Application::initWindow() {
    m_window = glfwCreateWindow(g_windowWidth, g_windowHeight, "Bring Me The Engine", nullptr, nullptr);
    if (!m_window) {
        const char * desc;
        int code = glfwGetError(&desc);
        std::cerr << "Erreur création fenêtre : " << (desc ? desc : "Erreur inconnue") << " (code " << code << ")\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0);
    return true;
}

bool Application::initGlad() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur : initialisation de GLAD échouée\n";
        return false;
    }
    return true;
}

void Application::setupCallbacks() {
    glfwSetKeyCallback(m_window, Callbacks::keyCallback);
    glfwSetCursorPosCallback(m_window, Callbacks::cursorPositionCallback);
}

void Application::setupScene() {
    g_sceneIndex = 1;
    reloadScene(g_sceneIndex);
    m_menu = std::make_unique<Menu>(m_window);
}

void Application::setupShaders() {
    const std::vector<std::pair<std::string, std::string>> shaderFiles = {
        {PathResolver::getResourcePath("shaders/main_shaders/vertex.vert"), PathResolver::getResourcePath("shaders/main_shaders/vertex.frag")},
        {PathResolver::getResourcePath("shaders/main_shaders/light_pos.vert"), PathResolver::getResourcePath("shaders/main_shaders/light_pos.frag")},
        {PathResolver::getResourcePath("shaders/main_shaders/skybox.vert"), PathResolver::getResourcePath("shaders/main_shaders/skybox.frag")},
        {PathResolver::getResourcePath("shaders/debug/bounding_box.vert"), PathResolver::getResourcePath("shaders/debug/bounding_box.frag")},
        {PathResolver::getResourcePath("shaders/shadows/dir_shadow.vert"), PathResolver::getResourcePath("shaders/shadows/dir_shadow.frag")},
        {PathResolver::getResourcePath("shaders/shadows/ponc_shadow.vert"), PathResolver::getResourcePath("shaders/shadows/ponc_shadow.frag")},
        {PathResolver::getResourcePath("shaders/main_shaders/Rendu_indirect/Gbuffer/Gbuffer.vert"), PathResolver::getResourcePath("shaders/main_shaders/Rendu_indirect/Gbuffer/Gbuffer.frag")},
        {PathResolver::getResourcePath("shaders/main_shaders/Rendu_indirect/Lighting_deferred/Lighting_deferred.vert"), PathResolver::getResourcePath("shaders/main_shaders/Rendu_indirect/Lighting_deferred/Lighting_deferred.frag")}
    };

    for (const auto & [vs, fs] : shaderFiles) {
        auto shader = std::make_unique<Shader>(vs, fs);

        if (!shader || shader->ID == 0) {
            std::cerr << "Erreur de compilation shader : " << vs << " / " << fs << "\n";
            exit(-1);
        }

        m_shaders.push_back(std::move(shader));
    }
}

void Application::cleanup() {
    m_menu.reset();
    g_scenePtr.reset();
    m_shaders.clear();
    TextureManager::clear();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}
