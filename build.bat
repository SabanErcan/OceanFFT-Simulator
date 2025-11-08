@echo off
REM Build script for Ocean FFT Simulator (Windows)

echo =========================================
echo   Ocean FFT Simulator - Build Script
echo =========================================
echo.

REM Check if vcpkg path is provided
if "%1"=="" (
    echo ERROR: Please provide vcpkg path as argument
    echo Usage: build.bat "C:\path\to\vcpkg"
    echo.
    echo Example: build.bat "C:\vcpkg"
    pause
    exit /b 1
)

set VCPKG_ROOT=%1
set VCPKG_TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake

echo Using vcpkg at: %VCPKG_ROOT%
echo.

REM Check if vcpkg exists
if not exist "%VCPKG_TOOLCHAIN%" (
    echo ERROR: vcpkg toolchain not found at %VCPKG_TOOLCHAIN%
    echo Please install vcpkg first
    pause
    exit /b 1
)

REM Create build directory
if not exist "build" (
    echo Creating build directory...
    mkdir build
)

cd build

REM Configure with CMake
echo.
echo [1/3] Configuring with CMake...
cmake .. -DCMAKE_TOOLCHAIN_FILE="%VCPKG_TOOLCHAIN%" -DCMAKE_BUILD_TYPE=Release

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed
    cd ..
    pause
    exit /b 1
)

REM Build
echo.
echo [2/3] Building project...
cmake --build . --config Release -j 8

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed
    cd ..
    pause
    exit /b 1
)

REM Success
echo.
echo [3/3] Build complete!
echo.
echo Executable location: build\Release\OceanFFT.exe
echo.
echo Run the simulator:
echo   cd build\Release
echo   .\OceanFFT.exe
echo.

cd ..
pause
