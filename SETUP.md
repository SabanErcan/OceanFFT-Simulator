# Setup Guide - Ocean FFT Simulator

## Quick Start (Windows + vcpkg)

### 1. Install vcpkg

```powershell
# Clone vcpkg
cd C:\
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# Bootstrap
.\bootstrap-vcpkg.bat

# Integrate with Visual Studio
.\vcpkg integrate install
```

### 2. Install Dependencies

```powershell
# Install required libraries (x64)
.\vcpkg install glfw3:x64-windows
.\vcpkg install fftw3:x64-windows
.\vcpkg install glm:x64-windows

# Verify installations
.\vcpkg list
```

### 3. Download ImGui

```powershell
# Download ImGui v1.89 or later
# https://github.com/ocornut/imgui/releases

# Extract to OceanFFT/include/imgui/
# Should contain:
#   - imgui.h, imgui.cpp
#   - imgui_draw.cpp, imgui_widgets.cpp, imgui_tables.cpp
#   - imgui_demo.cpp
#   - backends/imgui_impl_glfw.h/cpp
#   - backends/imgui_impl_opengl3.h/cpp
```

### 4. Generate GLAD

```powershell
# Go to https://glad.dav1d.de/
# Settings:
#   Language: C/C++
#   Specification: OpenGL
#   gl: Version 4.3+
#   Profile: Core
#   Options: [x] Generate a loader

# Download and extract:
#   include/glad/glad.h -> OceanFFT/include/glad/
#   include/KHR/khrplatform.h -> OceanFFT/include/KHR/
#   src/glad.c -> OceanFFT/src/
```

### 5. Build Project

```powershell
cd OceanFFT
mkdir build
cd build

# Configure with vcpkg
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"

# Build Release
cmake --build . --config Release

# Run
.\Release\OceanFFT.exe
```

## Linux Setup (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt update
sudo apt install -y build-essential cmake git
sudo apt install -y libglfw3-dev libfftw3-dev libglm-dev

# Download project
cd ~/Projects
git clone <your-repo-url> OceanFFT
cd OceanFFT

# Download ImGui (manual)
cd include
wget https://github.com/ocornut/imgui/archive/refs/tags/v1.89.9.tar.gz
tar -xzf v1.89.9.tar.gz
mv imgui-1.89.9 imgui
cd ..

# Generate GLAD (online at glad.dav1d.de or use glad-generator)
# Place files as described above

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run
./OceanFFT
```

## macOS Setup

```bash
# Install Homebrew if needed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake glfw fftw glm

# Download ImGui and GLAD (same as Linux)
# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)

./OceanFFT
```

## Troubleshooting

### "CMake Error: Could not find FFTW3"

**Windows**: Make sure vcpkg is integrated and toolchain file is specified
**Linux**: `sudo apt install libfftw3-dev`

### "glad.h: No such file or directory"

Download GLAD from https://glad.dav1d.de/ and place files correctly.

### "imgui.h: No such file or directory"

Download ImGui from GitHub releases and extract to `include/imgui/`

### Black screen / No ocean

Check console for shader errors. Shaders should be in `build/shaders/`

### Low FPS

- Reduce resolution in `Application.cpp`: `OceanFFT(128, 1000.0f)`
- Update GPU drivers
- Disable MSAA

## Verify Setup

After building, your directory should look like:

```
OceanFFT/
├── build/
│   ├── shaders/          ✓ Copied by CMake
│   │   ├── ocean.vert
│   │   └── ocean.frag
│   ├── Release/          ✓ Windows
│   │   └── OceanFFT.exe
│   └── OceanFFT          ✓ Linux/macOS
├── include/
│   ├── glad/
│   │   └── glad.h        ✓
│   ├── KHR/
│   │   └── khrplatform.h ✓
│   └── imgui/            ✓
│       ├── imgui.h
│       └── backends/
└── src/
    ├── glad.c            ✓
    └── ...
```

## Next Steps

Once compiled successfully:
1. Run the executable
2. Use WASD to move, mouse to look around
3. Press F1 to show UI panel
4. Experiment with parameters (wind speed, amplitude, etc.)
5. Try presets: "Calm Sea" and "Stormy Sea"

Enjoy! 🌊
