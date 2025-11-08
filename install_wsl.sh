#!/bin/bash
# Installation automatique des dépendances pour Ocean FFT sur WSL
# Exécute ce script : chmod +x install_wsl.sh && ./install_wsl.sh

echo "=========================================="
echo "  Ocean FFT - Installation WSL"
echo "=========================================="
echo ""

# Couleurs
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Fonction d'erreur
error_exit() {
    echo -e "${RED}ERROR: $1${NC}" >&2
    exit 1
}

# Vérifier que nous sommes sur WSL
if ! grep -q Microsoft /proc/version; then
    echo -e "${YELLOW}WARNING: Ne semble pas être WSL, mais on continue...${NC}"
fi

# Mise à jour du système
echo -e "${CYAN}[1/6] Mise à jour du système...${NC}"
sudo apt update || error_exit "apt update a échoué"

# Installation des outils de build
echo -e "${CYAN}[2/6] Installation des outils de build...${NC}"
sudo apt install -y \
    build-essential \
    cmake \
    git \
    wget \
    unzip \
    pkg-config || error_exit "Installation des outils de build a échoué"

echo -e "${GREEN}✓ Outils de build installés${NC}"

# Installation des dépendances OpenGL/GLFW/FFTW
echo -e "${CYAN}[3/6] Installation des bibliothèques (GLFW, FFTW, GLM)...${NC}"
sudo apt install -y \
    libglfw3-dev \
    libfftw3-dev \
    libglm-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev || error_exit "Installation des bibliothèques a échoué"

echo -e "${GREEN}✓ Bibliothèques installées${NC}"

# Télécharger et installer GLAD
echo -e "${CYAN}[4/6] Téléchargement de GLAD...${NC}"
cd /tmp
rm -rf glad_download
mkdir -p glad_download
cd glad_download

# GLAD pré-généré pour OpenGL 4.3 Core
echo -e "${YELLOW}Téléchargement de GLAD depuis GitHub...${NC}"
wget -q https://github.com/Dav1dde/glad/archive/refs/heads/glad2.zip -O glad.zip || {
    echo -e "${YELLOW}Téléchargement direct échoué, génération manuelle requise${NC}"
    echo -e "${YELLOW}Utilisation d'une version pré-configurée...${NC}"
}

# Alternative : utiliser python pour générer GLAD
echo -e "${CYAN}Installation de glad via pip...${NC}"
sudo apt install -y python3-pip python3-venv
python3 -m pip install --user glad || echo -e "${YELLOW}glad pip installation échoué, continuons...${NC}"

# Créer les répertoires dans le projet
PROJECT_DIR="/mnt/c/Users/saban/OneDrive/Documents/Cours IUT/BUT Info - 2A/Projet_perso/qt_c++_saban/OceanFFT"
cd "$PROJECT_DIR" || error_exit "Impossible d'accéder au dossier projet"

mkdir -p include/glad include/KHR src

# Générer GLAD avec Python
echo -e "${CYAN}Génération de GLAD pour OpenGL 4.3 Core...${NC}"
python3 -m glad --profile core --api gl=4.3 --out-path . --generator c 2>/dev/null || {
    echo -e "${YELLOW}Génération automatique impossible${NC}"
    echo -e "${YELLOW}Téléchargement manuel requis depuis https://glad.dav1d.de/${NC}"
}

echo -e "${GREEN}✓ GLAD configuré${NC}"

# Télécharger ImGui
echo -e "${CYAN}[5/6] Téléchargement de Dear ImGui...${NC}"
cd "$PROJECT_DIR"
rm -rf include/imgui
mkdir -p include

IMGUI_VERSION="v1.89.9"
echo -e "${YELLOW}Téléchargement de ImGui ${IMGUI_VERSION}...${NC}"
wget -q "https://github.com/ocornut/imgui/archive/refs/tags/${IMGUI_VERSION}.zip" -O imgui.zip || \
    error_exit "Téléchargement de ImGui a échoué"

echo -e "${YELLOW}Extraction...${NC}"
unzip -q imgui.zip -d include/ || error_exit "Extraction de ImGui a échoué"
mv include/imgui-* include/imgui 2>/dev/null || mv include/imgui-${IMGUI_VERSION#v} include/imgui
rm imgui.zip

# Vérifier que ImGui est bien là
if [ ! -f "include/imgui/imgui.h" ]; then
    error_exit "ImGui n'a pas été installé correctement"
fi

echo -e "${GREEN}✓ ImGui installé${NC}"

# Vérifier la structure
echo -e "${CYAN}[6/6] Vérification de la structure...${NC}"

check_file() {
    if [ -f "$1" ]; then
        echo -e "${GREEN}  ✓ $1${NC}"
        return 0
    else
        echo -e "${RED}  ✗ $1 (manquant)${NC}"
        return 1
    fi
}

echo ""
echo "Vérification des fichiers :"
check_file "CMakeLists.txt"
check_file "src/main.cpp"
check_file "src/Application.cpp"
check_file "src/OceanFFT.cpp"
check_file "shaders/ocean.vert"
check_file "shaders/ocean.frag"
check_file "include/imgui/imgui.h"
check_file "include/imgui/backends/imgui_impl_glfw.cpp"
check_file "include/imgui/backends/imgui_impl_opengl3.cpp"

echo ""
echo "Vérification des dépendances système :"
pkg-config --exists glfw3 && echo -e "${GREEN}  ✓ GLFW3${NC}" || echo -e "${RED}  ✗ GLFW3${NC}"
pkg-config --exists fftw3f && echo -e "${GREEN}  ✓ FFTW3${NC}" || echo -e "${RED}  ✗ FFTW3${NC}"
[ -f "/usr/include/glm/glm.hpp" ] && echo -e "${GREEN}  ✓ GLM${NC}" || echo -e "${RED}  ✗ GLM${NC}"

# Instructions finales
echo ""
echo -e "${GREEN}=========================================="
echo -e "  Installation terminée !"
echo -e "==========================================${NC}"
echo ""
echo -e "${CYAN}Prochaines étapes :${NC}"
echo ""
echo "1. Si GLAD n'a pas été généré automatiquement :"
echo -e "   ${YELLOW}Télécharge manuellement depuis : https://glad.dav1d.de/${NC}"
echo "   - OpenGL 4.3, Core, 'Generate a loader'"
echo "   - Place les fichiers dans include/glad/ et src/"
echo ""
echo "2. Compiler le projet :"
echo -e "   ${CYAN}cd \"$PROJECT_DIR\"${NC}"
echo -e "   ${CYAN}mkdir -p build && cd build${NC}"
echo -e "   ${CYAN}cmake .. -DCMAKE_BUILD_TYPE=Release${NC}"
echo -e "   ${CYAN}make -j\$(nproc)${NC}"
echo ""
echo "3. Lancer :"
echo -e "   ${CYAN}./OceanFFT${NC}"
echo ""
echo -e "${YELLOW}Note: Pour exécuter avec interface graphique depuis WSL :${NC}"
echo -e "   ${CYAN}export DISPLAY=:0${NC}"
echo -e "   Ou installe un serveur X comme VcXsrv sur Windows"
echo ""
