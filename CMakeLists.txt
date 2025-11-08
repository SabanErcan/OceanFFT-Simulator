cmake_minimum_required(VERSION 3.15)
project(OceanFFT LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(USE_VCPKG "Use vcpkg for dependencies" ON)

# Find packages
find_package(OpenGL REQUIRED)

# GLFW
find_package(glfw3 CONFIG REQUIRED)

# GLM
find_package(glm CONFIG REQUIRED)

# FFTW3
if(WIN32 AND USE_VCPKG)
    find_package(FFTW3 CONFIG REQUIRED)
    set(FFTW3_LIBRARIES FFTW3::fftw3f)
else()
    find_library(FFTW3F_LIBRARY fftw3f REQUIRED)
    find_path(FFTW3_INCLUDE_DIR fftw3.h REQUIRED)
    set(FFTW3_LIBRARIES ${FFTW3F_LIBRARY})
endif()

# ImGui sources
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Project sources
set(PROJECT_SOURCES
    src/main.cpp
    src/Application.cpp
    src/Camera.cpp
    src/OceanFFT.cpp
    src/OceanRenderer.cpp
    src/ShaderProgram.cpp
    src/Mesh.cpp
    src/glad.c
)

# Create executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${IMGUI_SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/glad
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${FFTW3_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    ${FFTW3_LIBRARIES}
)

# Copy shaders to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy assets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Windows specific settings
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
    )
    # Set as GUI application (no console)
    # set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

message(STATUS "===========================================")
message(STATUS "OceanFFT Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  OpenGL: ${OPENGL_LIBRARIES}")
message(STATUS "  FFTW3: ${FFTW3_LIBRARIES}")
message(STATUS "===========================================")
