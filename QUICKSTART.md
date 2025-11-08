# 🚀 Quick Start Guide - Ocean FFT Simulator

Get up and running in 10 minutes!

## Prerequisites Check

Do you have these installed?

- [ ] **CMake** (3.15+) - [Download](https://cmake.org/download/)
- [ ] **C++ Compiler** (MSVC 2019+, GCC 9+, or Clang 10+)
- [ ] **vcpkg** (Windows) or system package manager (Linux/macOS)

## Step 1: Clone/Download Project

You already have the source files in:
```
OceanFFT/
```

## Step 2: Install Dependencies

### Windows (5 minutes)

```powershell
# If you don't have vcpkg:
cd C:\
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# Install libraries
.\vcpkg install glfw3:x64-windows fftw3:x64-windows glm:x64-windows
```

### Linux (2 minutes)

```bash
sudo apt update
sudo apt install -y cmake build-essential libglfw3-dev libfftw3-dev libglm-dev
```

### macOS (3 minutes)

```bash
brew install cmake glfw fftw glm
```

## Step 3: Download GLAD (2 minutes)

1. Go to: **https://glad.dav1d.de/**
2. Select:
   - Language: `C/C++`
   - Specification: `OpenGL`
   - gl: `Version 4.3`
   - Profile: `Core`
   - [✓] Generate a loader
3. Click **GENERATE**
4. Download ZIP
5. Extract:
   - `include/glad/glad.h` → `OceanFFT/include/glad/glad.h`
   - `include/KHR/khrplatform.h` → `OceanFFT/include/KHR/khrplatform.h`
   - `src/glad.c` → `OceanFFT/src/glad.c`

## Step 4: Download ImGui (2 minutes)

1. Go to: **https://github.com/ocornut/imgui/releases**
2. Download latest release (v1.89+)
3. Extract entire folder to `OceanFFT/include/imgui/`
4. Make sure these exist:
   - `include/imgui/imgui.h`
   - `include/imgui/backends/imgui_impl_glfw.cpp`
   - `include/imgui/backends/imgui_impl_opengl3.cpp`

## Step 5: Build (3 minutes)

### Windows

```powershell
cd OceanFFT
.\build.bat "C:\vcpkg"
```

If error, build manually:
```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build . --config Release
```

### Linux/macOS

```bash
cd OceanFFT
chmod +x build.sh
./build.sh
```

Or manually:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Step 6: Run! 🎉

### Windows
```powershell
cd build\Release
.\OceanFFT.exe
```

### Linux/macOS
```bash
cd build
./OceanFFT
```

## First Time Usage

Once the window opens:

1. **Move camera**: WASD keys
2. **Look around**: Move mouse
3. **Speed boost**: Hold Shift
4. **Show UI**: Press F1
5. **Play with parameters**:
   - Increase "Wind Speed" → bigger waves
   - Increase "Choppiness" → sharper crests
   - Try "Stormy Sea" preset button

## Troubleshooting

### "CMake Error: Could not find FFTW3"
→ Re-run vcpkg install or check system packages

### "glad.h: No such file or directory"
→ Download GLAD (Step 3) and place in correct folder

### "imgui.h: No such file or directory"
→ Download ImGui (Step 4) and place in `include/imgui/`

### Black screen / No ocean
→ Check console for shader errors. Shaders should be in `build/shaders/`

### Low FPS (<20)
→ Edit `src/Application.cpp`, change line ~155:
```cpp
m_oceanFFT = std::make_unique<OceanFFT>(128, 1000.0f); // Reduce resolution
```

## File Checklist

Before building, verify these files exist:

```
OceanFFT/
├── CMakeLists.txt                    ✅
├── src/
│   ├── main.cpp                      ✅
│   ├── Application.cpp               ✅
│   ├── Camera.cpp                    ✅
│   ├── OceanFFT.cpp                  ✅
│   ├── OceanRenderer.cpp             ✅
│   ├── ShaderProgram.cpp             ✅
│   ├── Mesh.cpp                      ✅
│   └── glad.c                        ⚠️ DOWNLOAD
├── shaders/
│   ├── ocean.vert                    ✅
│   └── ocean.frag                    ✅
├── include/
│   ├── glad/
│   │   └── glad.h                    ⚠️ DOWNLOAD
│   ├── KHR/
│   │   └── khrplatform.h             ⚠️ DOWNLOAD
│   └── imgui/
│       ├── imgui.h                   ⚠️ DOWNLOAD
│       └── backends/                 ⚠️ DOWNLOAD
```

## Next Steps

Once running successfully:

1. Read `README.md` for full documentation
2. Explore `TECHNICAL_NOTES.md` to understand the math
3. Modify parameters in the UI
4. Try changing code in `OceanFFT.cpp` to experiment

## Need Help?

- **Build issues**: Check `SETUP.md` for detailed instructions
- **Technical questions**: Read `TECHNICAL_NOTES.md`
- **General info**: See `README.md`

---

**Estimated total time**: 15-20 minutes (mostly downloads)

**Once set up**: Instant builds, just run `build.bat` or `build.sh` again!

Enjoy your ocean simulator! 🌊🏄‍♂️
