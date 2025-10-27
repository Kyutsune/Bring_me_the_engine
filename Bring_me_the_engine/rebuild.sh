#!/bin/bash
set -e

echo "🔍 Vérification des dépendances..."

sudo apt-get update -qq
sudo apt-get install -y libglfw3-dev libglm-dev

echo "✅ Dépendances installées, compilation en cours..."

rm -rf build/
mkdir build
cd build  
cmake ../..
make -j$(nproc)
