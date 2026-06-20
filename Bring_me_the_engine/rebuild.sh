#!/bin/bash
set -e

echo "🔍 Vérification et installation des dépendances..."

# Charge les informations du système d'exploitation (/etc/os-release)
source /etc/os-release

# Détecte le gestionnaire de paquets et installe les dépendances
if [[ "$ID" == "debian" || "$ID" == "ubuntu" ]]; then
    echo "⚙️ Système détecté : Debian/Ubuntu. Utilisation de apt..."
    sudo apt-get update -qq
    sudo apt-get install -y libglfw3-dev libglm-dev

elif [[ "$ID" == "fedora" ]]; then
    echo "⚙️ Système détecté : Fedora. Utilisation de dnf..."
    # Note : Les noms de paquets peuvent être légèrement différents sur Fedora
    # - `glm-devel` pour libglm-dev
    # - `glfw-devel` ou `libglfw-devel` (vérifie si c'est la bonne version)
    sudo dnf install -y glm-devel glfw-devel

else
    echo "⚠️ Distribution non prise en charge pour l'installation automatique des dépendances. Veuillez installer manuellement libglfw3-dev et libglm-dev."
    exit 1
fi

echo "✅ Dépendances installées, compilation en cours..."

# --- Partie compilation inchangée ---
rm -rf build/
mkdir build
cd build
cmake ../..
make -j$(nproc)

echo "🎉 Compilation terminée avec succès !"
