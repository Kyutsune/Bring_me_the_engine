#include "system/UtilsFile.h"
#include "Globals.h"

namespace UtilsFile {
    void saveSettingsSensRot(const std::string& filepath) {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << g_sensibility_rot << "\n";
            file << g_sensibility_depl << "\n";
            file.close();
        }
        else
            std::cerr << "Erreur : Impossible d'ouvrir le fichier pour sauvegarder les paramètres de sensibilité.\n";
    }

    void loadSettingsSensRot(const std::string& filepath) {
        std::ifstream file(filepath);

        if (!file.is_open()) {
            std::cout << "Fichier de settings absent. Creation de : " << filepath << std::endl;
            g_sensibility_depl = 0.10f;
            g_sensibility_rot = 0.01f;
            saveSettingsSensRot(filepath);
            return;
        }

        if (!(file >> g_sensibility_rot >> g_sensibility_depl)) {
            std::cerr << "Erreur de lecture dans le fichier de settings." << std::endl;
        }

        file.close();
    }
}