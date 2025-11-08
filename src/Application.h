#pragma once

#include "Camera.h"
#include "OceanFFT.h"
#include "OceanRenderer.h"
#include <GLFW/glfw3.h>
#include <memory>

/**
 * @brief Main application class managing the simulation loop
 * 
 * Handles:
 * - Window and input management
 * - ImGui UI
 * - Ocean simulation and rendering
 * - Frame timing
 */
class Application {
public:
    Application();
    ~Application();

    /**
     * @brief Initialize application (GLFW, OpenGL, ImGui)
     * @param width Window width
     * @param height Window height
     * @param title Window title
     * @return true if successful
     */
    bool initialize(int width, int height, const char* title);

    /**
     * @brief Main application loop
     */
    void run();

    /**
     * @brief Check if application should close
     */
    bool shouldClose() const;

private:
    // GLFW window
    GLFWwindow* m_window;
    int m_windowWidth;
    int m_windowHeight;

    // Core components
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<OceanFFT> m_oceanFFT;
    std::unique_ptr<OceanRenderer> m_renderer;

    // Timing
    float m_deltaTime;
    float m_lastFrame;
    float m_simTime;
    float m_timeScale;
    int m_frameCount;  // Pour optimisation FFT

    // Input state
    bool m_firstMouse;
    float m_lastMouseX;
    float m_lastMouseY;
    bool m_mouseCaptured;

    // UI state
    bool m_showUI;
    bool m_showStats;

    // Ocean parameters (UI controlled)
    struct OceanParams {
        float windSpeed = 30.0f;
        float windDirection[2] = {1.0f, 0.0f};
        float amplitude = 0.0002f;
        float choppy = 2.0f;
        float waterColor[3] = {0.0f, 0.3f, 0.5f};
        float foamThreshold = 0.5f;
        bool wireframe = false;
    } m_params;

    // Methods

    /**
     * @brief Initialize GLFW and create window
     */
    bool initWindow(int width, int height, const char* title);

    /**
     * @brief Initialize GLAD (OpenGL loader)
     */
    bool initGLAD();

    /**
     * @brief Initialize ImGui
     */
    bool initImGui();

    /**
     * @brief Initialize ocean simulation and renderer
     */
    bool initOcean();

    /**
     * @brief Process input events
     */
    void processInput();

    /**
     * @brief Update simulation
     */
    void update();

    /**
     * @brief Render frame
     */
    void render();

    /**
     * @brief Render ImGui UI
     */
    void renderUI();

    /**
     * @brief Cleanup resources
     */
    void cleanup();

    // GLFW callbacks (static)
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
