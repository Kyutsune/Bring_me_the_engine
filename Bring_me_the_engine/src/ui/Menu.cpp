#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include "system/PathResolver.h"
#include "ui/Menu.h"
#include "ui/Sections.h"

Menu::Menu(GLFWwindow * window) : window(window), scene(g_scene) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark(); // ou .Classic(), .Light()

    ImGuiStyle & style = ImGui::GetStyle();
    style.ScaleAllSizes(1.5f);

    ImGuiIO & io = ImGui::GetIO();
    io.FontGlobalScale = 1.5f;
    static std::string iniFilePath = PathResolver::getResourcePath("build/menu.ini").string();
    io.IniFilename = iniFilePath.c_str();

    ImGui::LoadIniSettingsFromDisk(io.IniFilename);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410 core");

    m_taille_minimale_x = 300;
    m_taille_minimale_y = 100;
    m_taille_maximale_x = 800;
    m_taille_maximale_y = 400;
}

Menu::~Menu() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Menu::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Menu::setupMenuDisplay() {
    // On force la taille de la fenêtre à la taille minimale nécessaire pour afficher le contenu
    ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    // Et on fixe des contraintes maximales et minimales pour la taille de ce menu
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(m_taille_minimale_x, m_taille_minimale_y),
        ImVec2(m_taille_maximale_x, m_taille_maximale_y));
}

void Menu::render() {
    if (!m_menu_principal)
        return;

    /// Partie du menu qui gère tout ce qui touche à la scène déjà existante, on pourrait dire le menu "principal"
    setupMenuDisplay();
    ImGui::Begin("Bring me the menu", &m_menu_principal);

    if (Sections::SceneSection(scene)) {
        scene = &getScene();
    }

    Sections::objectSection(scene);
    Sections::textureSection(scene);
    Sections::lightSection(scene);
    Sections::fogSection(scene);
    Sections::sensitivitySection();
    Sections::quitButton(window);

    // Deuxième partie du menu qui conditionnera elle tout ce qui touche à l'édition de la scène,
    // dont le fait de rajouter des entités, textures etc...
    setupMenuDisplay();
    ImGui::Begin("Editeur de scène", &m_menu_principal);
    Sections::changeEntityCreatedSection(scene);

    setupMenuDisplay();
    ImGui::Begin("Performances", &m_menu_principal);
    Sections::performanceSection();

    ImGui::End();
    ImGui::End();
    ImGui::End();
}

void Menu::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
