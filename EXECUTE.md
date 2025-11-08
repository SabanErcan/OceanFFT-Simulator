# 🚀 Guide d'Exécution Rapide - Ocean FFT

## Étape 1 : Télécharger GLAD

### Option A : Automatique (Recommandé)
Exécute ce script PowerShell pour télécharger GLAD automatiquement :

```powershell
# Créer les dossiers
New-Item -ItemType Directory -Force -Path "include\glad"
New-Item -ItemType Directory -Force -Path "include\KHR"

# Note : GLAD doit être téléchargé manuellement depuis https://glad.dav1d.de/
# Configuration : OpenGL 4.3, Core Profile, Generate a loader
Write-Host "Veuillez télécharger GLAD depuis : https://glad.dav1d.de/" -ForegroundColor Yellow
```

### Option B : Manuel (Plus simple)

1. Va sur : **https://glad.dav1d.de/**
2. Configure :
   - Language : `C/C++`
   - Specification : `OpenGL`
   - gl : `Version 4.3`
   - Profile : `Core`
   - ✅ Coche `Generate a loader`
3. Clique sur **GENERATE**
4. Télécharge le ZIP
5. Extrait les fichiers :
   - `include/glad/glad.h` → `OceanFFT/include/glad/glad.h`
   - `include/KHR/khrplatform.h` → `OceanFFT/include/KHR/khrplatform.h`
   - `src/glad.c` → `OceanFFT/src/glad.c`

## Étape 2 : Télécharger ImGui

```powershell
# PowerShell - Téléchargement automatique
$imguiVersion = "v1.89.9"
$url = "https://github.com/ocornut/imgui/archive/refs/tags/$imguiVersion.zip"
$output = "imgui.zip"

Write-Host "Téléchargement de ImGui $imguiVersion..." -ForegroundColor Cyan
Invoke-WebRequest -Uri $url -OutFile $output

Write-Host "Extraction..." -ForegroundColor Cyan
Expand-Archive -Path $output -DestinationPath "include" -Force
Rename-Item -Path "include\imgui-*" -NewName "imgui" -Force
Remove-Item $output

Write-Host "ImGui installé avec succès !" -ForegroundColor Green
```

**OU manuellement :**
1. https://github.com/ocornut/imgui/releases
2. Télécharge `Source code (zip)`
3. Extrait dans `include/imgui/`

## Étape 3 : Installer vcpkg (si pas déjà fait)

```powershell
# Dans C:\
cd C:\
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

## Étape 4 : Installer les dépendances

```powershell
cd C:\vcpkg
.\vcpkg install glfw3:x64-windows
.\vcpkg install fftw3:x64-windows
.\vcpkg install glm:x64-windows

# Vérification
.\vcpkg list
```

## Étape 5 : Compiler le projet

```powershell
cd "c:\Users\saban\OneDrive\Documents\Cours IUT\BUT Info - 2A\Projet_perso\qt_c++_saban\OceanFFT"

# Utilise le script de build
.\build.bat "C:\vcpkg"

# OU manuellement :
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build . --config Release -j 8
```

## Étape 6 : Lancer !

```powershell
cd build\Release
.\OceanFFT.exe
```

## 🎮 Contrôles une fois lancé

- **WASD** : Déplacer la caméra
- **Souris** : Regarder autour
- **Shift** : Boost de vitesse
- **Space** : Monter
- **C** : Descendre
- **F1** : Afficher/masquer l'interface
- **Tab** : Capturer/libérer la souris
- **ESC** : Quitter

## ⚡ Script Tout-en-Un (PowerShell)

Sauvegarde ce script comme `setup.ps1` et exécute-le :

```powershell
# setup.ps1 - Configuration automatique du projet Ocean FFT

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Ocean FFT - Configuration Automatique" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Vérifier vcpkg
$vcpkgPath = "C:\vcpkg"
if (-not (Test-Path $vcpkgPath)) {
    Write-Host "vcpkg n'est pas installé. Installation..." -ForegroundColor Yellow
    cd C:\
    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    .\bootstrap-vcpkg.bat
    .\vcpkg integrate install
    cd $PSScriptRoot
} else {
    Write-Host "✓ vcpkg trouvé" -ForegroundColor Green
}

# Installer dépendances
Write-Host ""
Write-Host "Installation des dépendances..." -ForegroundColor Cyan
cd $vcpkgPath
.\vcpkg install glfw3:x64-windows fftw3:x64-windows glm:x64-windows

# Télécharger ImGui
Write-Host ""
Write-Host "Téléchargement de ImGui..." -ForegroundColor Cyan
cd $PSScriptRoot
$imguiUrl = "https://github.com/ocornut/imgui/archive/refs/tags/v1.89.9.zip"
Invoke-WebRequest -Uri $imguiUrl -OutFile "imgui.zip"
Expand-Archive -Path "imgui.zip" -DestinationPath "include" -Force
Get-ChildItem "include\imgui-*" | Rename-Item -NewName "imgui" -Force
Remove-Item "imgui.zip"

Write-Host "✓ ImGui installé" -ForegroundColor Green

# Instructions GLAD
Write-Host ""
Write-Host "ATTENTION : GLAD doit être téléchargé manuellement" -ForegroundColor Yellow
Write-Host "1. Va sur : https://glad.dav1d.de/" -ForegroundColor Yellow
Write-Host "2. Configure : OpenGL 4.3, Core, Generate a loader" -ForegroundColor Yellow
Write-Host "3. Télécharge et place les fichiers dans include/glad/ et src/" -ForegroundColor Yellow
Write-Host ""
Write-Host "Appuie sur une touche une fois GLAD installé..." -ForegroundColor Yellow
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

# Compiler
Write-Host ""
Write-Host "Compilation du projet..." -ForegroundColor Cyan
.\build.bat $vcpkgPath

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  Configuration terminée !" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Lance le programme avec :" -ForegroundColor Cyan
Write-Host "  cd build\Release" -ForegroundColor White
Write-Host "  .\OceanFFT.exe" -ForegroundColor White
```

## 🆘 Dépannage Rapide

### Erreur : "FFTW3 not found"
```powershell
cd C:\vcpkg
.\vcpkg install fftw3:x64-windows --recurse
```

### Erreur : "glad.h not found"
- Télécharge GLAD depuis https://glad.dav1d.de/
- Place les fichiers aux bons emplacements

### Erreur : "imgui.h not found"
```powershell
# Retélécharge ImGui
Invoke-WebRequest -Uri "https://github.com/ocornut/imgui/archive/refs/tags/v1.89.9.zip" -OutFile "imgui.zip"
Expand-Archive -Path "imgui.zip" -DestinationPath "include" -Force
```

### FPS très bas
Édite `src/Application.cpp` ligne ~155 :
```cpp
m_oceanFFT = std::make_unique<OceanFFT>(128, 1000.0f); // Au lieu de 256
```

## ✅ Checklist de Vérification

Avant de compiler, vérifie que ces fichiers existent :

```
OceanFFT/
├── include/
│   ├── glad/
│   │   └── glad.h                    ⚠️ À télécharger
│   ├── KHR/
│   │   └── khrplatform.h             ⚠️ À télécharger
│   └── imgui/
│       ├── imgui.h                   ⚠️ À télécharger
│       └── backends/                 ⚠️ À télécharger
└── src/
    └── glad.c                        ⚠️ À télécharger
```

Tous les autres fichiers sont déjà créés ! ✅

---

**Temps estimé total : 15-20 minutes**

Bon courage ! 🚀🌊
