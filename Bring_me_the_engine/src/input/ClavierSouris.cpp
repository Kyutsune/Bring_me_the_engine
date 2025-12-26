#include "input/ClavierSouris.h"
#include "Globals.h"
#include "imgui.h"
#include "input/EntityCreator.h"
#include "math/intersect/IntersectInfo.h"
#include "math/intersect/IntersectUtils.h"
#include <iostream>

void reactKeyboardPressed(GLFWwindow * window, const char * nameKeyPressed, int keyPressed) {
     //std::cout<<nameKeyPressed << " pressed" << std::endl;
    // Gestion pour fermer la fenêtre si la touche Échap est pressée
    if (keyPressed == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }
    if (keyPressed == GLFW_KEY_R) {
		g_renderer->toggleRenderType();
    }
}

namespace ClavierSouris {

    // Variables statiques internes au namespace
    static double m_lastX = 0.0;
    static double m_lastY = 0.0;
    static bool m_firstMouse = true;

    void update(GLFWwindow * window) {
        handleContinuousInput(window);
        handleMouseLeftClick(window);
    }

    void handleContinuousInput(GLFWwindow * window) {
        if (!g_scene)
            return;

        Camera * cam = g_scene->getCamera();
        if (!cam)
            return;

        // FIXME: J'utilise du hardcode pour les entiers de touches car le système de GLFW est sur le qwerty et pas azerty.
        //  Il pourrait y avoir d'autres moyens plus propre pour gérer cela?
        //  Voici la solution de Hazel par exemple :
        //  https://github.com/TheCherno/Hazel/blob/1feb70572fa87fa1c4ba784a2cfeada5b4a500db/Hazel/src/Hazel/Core/KeyCodes.h
        if (glfwGetKey(window, 87) == GLFW_PRESS) { // z
            Vec3 dir = cam->getForward();
            cam->setPosition(cam->getPosition() + dir * g_sensibility_depl);
            cam->setTarget(cam->getTarget() + dir * g_sensibility_depl);
        }

        if (glfwGetKey(window, 83) == GLFW_PRESS) { // s
            Vec3 dir = cam->getBackward();
            cam->setPosition(cam->getPosition() + dir * g_sensibility_depl);
            cam->setTarget(cam->getTarget() + dir * g_sensibility_depl);
        }

        if (glfwGetKey(window, 81) == GLFW_PRESS) { // a
            Vec3 dir = cam->getLeft();
            cam->setPosition(cam->getPosition() + dir * g_sensibility_depl);
            cam->setTarget(cam->getTarget() + dir * g_sensibility_depl);
        }

        if (glfwGetKey(window, 68) == GLFW_PRESS) { // d
            Vec3 dir = cam->getRight();
            cam->setPosition(cam->getPosition() + dir * g_sensibility_depl);
            cam->setTarget(cam->getTarget() + dir * g_sensibility_depl);
        }

        if (glfwGetKey(window, 67) == GLFW_PRESS) { // c
            std::cout << "Position de la caméra: " << cam->getPosition() << std::endl;
        }
    }

    void handleMouseLeftClick(GLFWwindow * window) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (ImGui::GetIO().WantCaptureMouse)
            return;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            gestionClicGauche(xpos, ypos);
        }
    }

    void handleMouseMovement(GLFWwindow * window, double xpos, double ypos) {
        if (!g_scene)
            return;

        Camera * cam = g_scene->getCamera();
        if (!cam)
            return;

        if (m_firstMouse) {
            m_lastX = xpos;
            m_lastY = ypos;
            m_firstMouse = false;
            return;
        }

        double dx = xpos - m_lastX;
        double dy = ypos - m_lastY;

        // Déplacement latéral + vertical avec Ctrl
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {

            cam->setPosition(cam->getPosition() - cam->getRight() * static_cast<float>(dx) * g_sensibility_depl);
            cam->setTarget(cam->getTarget() - cam->getRight() * static_cast<float>(dx) * g_sensibility_depl);

            cam->setPosition(cam->getPosition() + Vec3(0, 1, 0) * static_cast<float>(dy) * g_sensibility_depl);
            cam->setTarget(cam->getTarget() + Vec3(0, 1, 0) * static_cast<float>(dy) * g_sensibility_depl);
        }

        // Rotation caméra avec clic droit
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            Vec3 dir = cam->getDirection();
            Vec3 right = cam->getRight();
            Vec3 up = Vec3(0, 1, 0);

            float yaw = static_cast<float>(dx) * g_sensibility_rot;
            float pitch = static_cast<float>(dy) * g_sensibility_rot;

            Mat4 yawRotation;
            yawRotation = yawRotation.rotate(up, yaw);

            Mat4 pitchRotation;
            pitchRotation = pitchRotation.rotate(right, pitch);

            Vec3 rotatedDir = pitchRotation * (yawRotation * dir);

            cam->setTarget(cam->getPosition() + rotatedDir.normalized());
        }

        m_lastX = xpos;
        m_lastY = ypos;
    }

    // FIXME: Pas vraiment un fixme mais cette fonction devrait vraiment être gérée par ClavierSouris??
    // Certes c'est le clic qui déclenche cela mais le fait que ce soit un test de collision entre rayon
    // et entité n'impliquerait pas que cela doive aller dans la struct Ray voir Entity?
    void gestionClicGauche(double x, double y) {
        Ray ray = Ray::generateRayFromScreen(x, y);
        const Scene & scene = getScene();
        std::vector<std::shared_ptr<Entity>> entities = scene.getEntities();
        const std::vector<std::shared_ptr<Entity>> & lightentities = scene.getLightEntities();
        entities.insert(entities.end(), lightentities.begin(), lightentities.end());

        IntersectionInfo retour_info;

        // Ici on va récupérer l'entité la plus proche de la caméra dans la direction du clic
        for (const auto & entity : entities) {
            if (!entity->isVisible())
                continue;

            IntersectUtils::intersectEntity(ray, *entity, retour_info, 10000.0f);
        }

        if (retour_info.hit && retour_info.entity) {

            std::cout<<"Clic gauche sur l'entité: " << retour_info.entity->getName()
                     << " à la position " << retour_info.position << std::endl;
            // Si sol cliqué, alors on va vouloir créer une entité si possible en l'endroit du clic
            // TODO: Il faudrait un moyen plus propre de savoir si on a cliqué sur le sol ou pas
            // Ici on suppose que le sol est l'entité nommée "Sol_beton" ce qui n'est pas générique
            if (retour_info.entity->getName() == "Sol_beton") {
                EntityCreator t_entityCreator;

                // Boucle qui va vérifier si une entité existe déjà à l'endroit du clic ou à proximité directe
                bool entityExists = false;
                for (const auto & e : getScene().getEntities()) {
                    if (e == retour_info.entity)
                        continue;

                    if ((e->getPosition() - retour_info.position).length() <= 1.5f) {
                        entityExists = true;
                        break;
                    }
                }

                if (entityExists)
                    return;

                // Si aucune entité n'existe à cet endroit, on crée une nouvelle entité
                std::shared_ptr<Entity> t_entity_ret = t_entityCreator.createEntity(
                    g_typeEntityCreated,
                    retour_info.position.x,
                    retour_info.entity->getPosition().y,
                    retour_info.position.z);

                g_entityExpanded.clear();
                g_entityExpanded[t_entity_ret->getName()] = true;
                g_forceOpenObjectHeader = true;
                return;
            }
            if (retour_info.entity->getName().rfind("Light_", 0) == 0) {
                std::cout << "Une lumière a été cliquée: " << retour_info.entity->getName() << std::endl;
                g_lightExpanded.clear();
                g_lightExpanded[retour_info.entity->getName()] = true;
                g_forceOpenLightHeader = true;
                return;
            } else {
                // On est pas sur le sol, on séléctionne l'entité sur laquelle on a cliqué
                // AVENIR: Quand le système de séléction sera implémenté, il suffira de sortir ce bout de code
                // et de le conditionner au fait qu'on ait choisir la séléction plutôt que la création d'entité
                g_entityExpanded.clear();
                g_entityExpanded[retour_info.entity->getName()] = true;
                g_forceOpenObjectHeader = true;
                return;
            }
        }

        // Si aucune intersection avec une entité, on va créer une entité en y=0 (convention du moteur)
        Vec3 hitPoint;
        if (IntersectUtils::intersectRayWithPlaneY0(ray, hitPoint)) {
            EntityCreator t_entityCreator;
            t_entityCreator.createEntity(g_typeEntityCreated, hitPoint.x, hitPoint.y, hitPoint.z);
        }
    }
}