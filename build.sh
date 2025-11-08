#!/bin/bash
# Build script for Ocean FFT Simulator (Linux/macOS)

echo "========================================="
echo "  Ocean FFT Simulator - Build Script"
echo "========================================="
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check dependencies
echo "Checking dependencies..."

command -v cmake >/dev/null 2>&1 || {
    echo -e "${RED}ERROR: cmake not found${NC}"
    echo "Install with: sudo apt install cmake (Ubuntu) or brew install cmake (macOS)"
    exit 1
}

command -v pkg-config >/dev/null 2>&1 || {
    echo -e "${YELLOW}WARNING: pkg-config not found${NC}"
}

# Check for libraries (Linux)
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    pkg-config --exists glfw3 || {
        echo -e "${YELLOW}WARNING: glfw3 not found${NC}"
        echo "Install with: sudo apt install libglfw3-dev"
    }
    
    pkg-config --exists fftw3f || {
        echo -e "${YELLOW}WARNING: fftw3 not found${NC}"
        echo "Install with: sudo apt install libfftw3-dev"
    }
fi

echo -e "${GREEN}✓ Dependencies OK${NC}"
echo ""

# Create build directory
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# Configure
echo ""
echo "[1/3] Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: CMake configuration failed${NC}"
    cd ..
    exit 1
fi

# Build
echo ""
echo "[2/3] Building project..."
CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
make -j$CORES

if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Build failed${NC}"
    cd ..
    exit 1
fi

# Success
echo ""
echo -e "${GREEN}[3/3] Build complete!${NC}"
echo ""
echo "Executable location: build/OceanFFT"
echo ""
echo "Run the simulator:"
echo "  cd build"
echo "  ./OceanFFT"
echo ""

cd ..
