# 🐧 Guide WSL - Ocean FFT Simulator

Configuration et compilation sur Windows Subsystem for Linux (Ubuntu)

## Pourquoi WSL ?

✅ Plus simple pour installer les dépendances  
✅ Pas besoin de vcpkg  
✅ Gestion de packages avec apt  
✅ Compilation plus rapide  
✅ Meilleure compatibilité OpenGL

## Configuration WSL

### 1. Installer WSL (si pas déjà fait)

Ouvre PowerShell en **Administrateur** :

```powershell
wsl --install
# OU si déjà installé
wsl --install -d Ubuntu
```

Redémarre Windows après l'installation.

### 2. Lancer WSL

```powershell
wsl
```

## Installation Automatique (Recommandé)

### Méthode 1 : Script tout-en-un

```bash
# Dans WSL, accède au projet
cd "/mnt/c/Users/saban/OneDrive/Documents/Cours IUT/BUT Info - 2A/Projet_perso/qt_c++_saban/OceanFFT"

# Rend le script exécutable
chmod +x install_wsl.sh

# Lance l'installation
./install_wsl.sh
```

Ce script va :
- ✅ Mettre à jour le système
- ✅ Installer CMake, GCC, Make
- ✅ Installer GLFW3, FFTW3, GLM
- ✅ Télécharger ImGui
- ✅ Configurer GLAD (headers)
- ✅ Vérifier la structure du projet

**Temps estimé : 5-10 minutes**

## Installation Manuelle (Détaillée)

### 1. Mise à jour système

```bash
sudo apt update
sudo apt upgrade -y
```

### 2. Outils de développement

```bash
sudo apt install -y \
    build-essential \
    cmake \
    git \
    wget \
    unzip \
    pkg-config
```

### 3. Bibliothèques graphiques

```bash
sudo apt install -y \
    libglfw3-dev \
    libfftw3-dev \
    libglm-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev
```

### 4. ImGui

```bash
cd "/mnt/c/Users/saban/OneDrive/Documents/Cours IUT/BUT Info - 2A/Projet_perso/qt_c++_saban/OceanFFT"

# Télécharger
wget https://github.com/ocornut/imgui/archive/refs/tags/v1.89.9.zip -O imgui.zip

# Extraire
unzip imgui.zip -d include/
mv include/imgui-* include/imgui
rm imgui.zip
```

### 5. GLAD

**Option A : Script automatique**
```bash
chmod +x download_glad.sh
./download_glad.sh
```

**Option B : Manuel**

Télécharge depuis https://glad.dav1d.de/ :
- OpenGL : 4.3
- Profile : Core
- ✅ Generate a loader

Extrait :
- `include/glad/glad.h` → `include/glad/glad.h`
- `include/KHR/khrplatform.h` → `include/KHR/khrplatform.h`
- `src/glad.c` → `src/glad.c`

## Compilation

### Première compilation

```bash
cd "/mnt/c/Users/saban/OneDrive/Documents/Cours IUT/BUT Info - 2A/Projet_perso/qt_c++_saban/OceanFFT"

# Créer dossier build
mkdir -p build
cd build

# Configurer avec CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_VCPKG=OFF

# Compiler (utilise tous les cœurs)
make -j$(nproc)
```

### Compilations suivantes (rapide)

```bash
cd build
make -j$(nproc)
```

### Nettoyage

```bash
# Dans build/
make clean

# OU complètement
cd ..
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_VCPKG=OFF
make -j$(nproc)
```

## Exécution

### Option 1 : Avec X Server (Interface Graphique)

**Installer VcXsrv sur Windows :**
1. Télécharge https://sourceforge.net/projects/vcxsrv/
2. Lance XLaunch
3. Configuration :
   - Multiple windows
   - Display number: 0
   - Start no client
   - ✅ Disable access control

**Dans WSL :**
```bash
export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0

cd build
./OceanFFT
```

### Option 2 : Compilation pour Windows

Si problème avec X Server, compile nativement pour Windows :

```bash
# Dans WSL
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_SYSTEM_NAME=Windows \
         -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
         -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++

# Installe MinGW si nécessaire
sudo apt install -y mingw-w64
```

Puis lance l'exe depuis Windows Explorer.

## Vérification de l'installation

### Script de vérification

```bash
cd "/mnt/c/Users/saban/OneDrive/Documents/Cours IUT/BUT Info - 2A/Projet_perso/qt_c++_saban/OceanFFT"

echo "Vérification des dépendances..."

# Vérifier les bibliothèques
echo -n "GLFW3: "
pkg-config --exists glfw3 && echo "✓" || echo "✗"

echo -n "FFTW3: "
pkg-config --exists fftw3f && echo "✓" || echo "✗"

echo -n "GLM: "
[ -f "/usr/include/glm/glm.hpp" ] && echo "✓" || echo "✗"

# Vérifier les fichiers du projet
echo ""
echo "Fichiers du projet:"
[ -f "CMakeLists.txt" ] && echo "✓ CMakeLists.txt" || echo "✗ CMakeLists.txt"
[ -f "src/main.cpp" ] && echo "✓ src/main.cpp" || echo "✗ src/main.cpp"
[ -f "src/glad.c" ] && echo "✓ src/glad.c" || echo "✗ src/glad.c (À télécharger)"
[ -f "include/imgui/imgui.h" ] && echo "✓ ImGui" || echo "✗ ImGui"
[ -f "include/glad/glad.h" ] && echo "✓ GLAD headers" || echo "✗ GLAD headers"
```

## Dépannage WSL

### "Permission denied" lors de l'exécution

```bash
chmod +x install_wsl.sh
chmod +x download_glad.sh
```

### "Package not found"

```bash
sudo apt update
sudo apt upgrade
# Puis réinstalle les packages
```

### "CMake Error: Could not find..."

```bash
# Vérifier PKG_CONFIG_PATH
export PKG_CONFIG_PATH=/usr/lib/pkgconfig:/usr/local/lib/pkgconfig

# Réinstaller la bibliothèque manquante
sudo apt install --reinstall lib<nom>-dev
```

### Problème d'affichage X11

```bash
# Vérifier DISPLAY
echo $DISPLAY

# Devrait afficher quelque chose comme: 192.168.x.x:0

# Si vide, configurer:
export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0

# Ajouter à ~/.bashrc pour permanence
echo 'export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '"'"'{print $2}'"'"'):0' >> ~/.bashrc
```

### glad.c manquant

```bash
# Option 1: Télécharger avec wget (si URL disponible)
cd src/
wget <URL_VERS_GLAD_C>

# Option 2: Depuis Windows
# Va sur https://glad.dav1d.de/, génère, et copie glad.c dans src/
```

## Performance WSL vs Windows natif

| Aspect | WSL | Windows natif |
|--------|-----|---------------|
| Installation dépendances | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| Compilation | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| Performance runtime | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Facilité setup | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |

**Recommandation :** Utilise WSL pour développer/compiler, lance nativement sur Windows pour le meilleur FPS.

## Commandes Rapides

```bash
# Aller au projet
alias oceanfft='cd "/mnt/c/Users/saban/OneDrive/Documents/Cours IUT/BUT Info - 2A/Projet_perso/qt_c++_saban/OceanFFT"'

# Compiler rapidement
alias ocean-build='cd build && make -j$(nproc)'

# Lancer
alias ocean-run='cd build && export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '"'"'{print $2}'"'"'):0 && ./OceanFFT'

# Ajoute à ~/.bashrc :
echo "alias oceanfft='cd \"/mnt/c/Users/saban/OneDrive/Documents/Cours IUT/BUT Info - 2A/Projet_perso/qt_c++_saban/OceanFFT\"'" >> ~/.bashrc
```

## Résumé : Installation en 3 étapes

```bash
# 1. Lancer WSL
wsl

# 2. Aller au projet et installer
cd "/mnt/c/Users/saban/OneDrive/Documents/Cours IUT/BUT Info - 2A/Projet_perso/qt_c++_saban/OceanFFT"
chmod +x install_wsl.sh && ./install_wsl.sh

# 3. Compiler
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_VCPKG=OFF
make -j$(nproc)

# 4. (Si glad.c manque) Télécharger depuis https://glad.dav1d.de/
#    et placer dans src/glad.c, puis refaire make
```

**C'est tout ! 🎉**

---

Pour plus d'infos : voir `EXECUTE.md` (méthode Windows) ou `README.md` (documentation complète)
