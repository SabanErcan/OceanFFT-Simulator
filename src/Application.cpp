#include "Application.h"
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

Application::Application()
    : m_window(nullptr)
    , m_windowWidth(1920)
    , m_windowHeight(1080)
    , m_deltaTime(0.0f)
    , m_lastFrame(0.0f)
    , m_simTime(0.0f)
    , m_timeScale(1.0f)
    , m_frameCount(0)
    , m_firstMouse(true)
    , m_lastMouseX(0.0f)
    , m_lastMouseY(0.0f)
    , m_mouseCaptured(true)
    , m_showUI(true)
    , m_showStats(true) {
}

Application::~Application() {
    cleanup();
}

bool Application::initialize(int width, int height, const char* title) {
    m_windowWidth = width;
    m_windowHeight = height;

    if (!initWindow(width, height, title)) return false;
    if (!initGLAD()) return false;
    
    // Set GLFW callbacks BEFORE ImGui so ImGui can chain them
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetCursorPosCallback(m_window, mouseCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
    glfwSetKeyCallback(m_window, keyCallback);
    
    if (!initImGui()) return false;
    if (!initOcean()) return false;

    // Create camera
    m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 50.0f, 200.0f));
    m_camera->setMovementSpeed(50.0f);

    // Capture mouse initially
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // Sky blue

    std::cout << "Application initialized successfully\n";
    std::cout << "Controls:\n";
    std::cout << "  WASD - Move camera\n";
    std::cout << "  Mouse - Look around\n";
    std::cout << "  Shift - Speed boost\n";
    std::cout << "  Space - Move up\n";
    std::cout << "  C - Move down\n";
    std::cout << "  Tab - Toggle mouse capture\n";
    std::cout << "  F1 - Toggle UI\n";
    std::cout << "  ESC - Exit\n";

    return true;
}

void Application::run() {
    while (!shouldClose()) {
        // Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        // Update simulation time
        m_simTime += m_deltaTime * m_timeScale;

        // Process input, update, and render
        processInput();
        update();
        render();

        // Swap buffers and poll events
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

bool Application::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

bool Application::initWindow(int width, int height, const char* title) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "ERROR: Failed to initialize GLFW\n";
        return false;
    }

    // OpenGL version 4.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA

    // Create window
    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        std::cerr << "ERROR: Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // VSync

    std::cout << "Window created (" << width << "x" << height << ")\n";
    return true;
}

bool Application::initGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "ERROR: Failed to initialize GLAD\n";
        return false;
    }

    std::cout << "OpenGL " << GLVersion.major << "." << GLVersion.minor << "\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
    return true;
}

bool Application::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    std::cout << "ImGui initialized\n";
    return true;
}

bool Application::initOcean() {
    // Create ocean FFT simulation (128x128 resolution, 1000m patch)
    // Résolution réduite pour améliorer les performances (256->128 = 4x plus rapide)
    m_oceanFFT = std::make_unique<OceanFFT>(128, 1000.0f);
    
    if (!m_oceanFFT->initialize()) {
        std::cerr << "ERROR: Failed to initialize OceanFFT\n";
        return false;
    }

    // Set initial parameters
    m_oceanFFT->setWindSpeed(m_params.windSpeed);
    m_oceanFFT->setWindDirection(glm::vec2(m_params.windDirection[0], m_params.windDirection[1]));
    m_oceanFFT->setAmplitude(m_params.amplitude);
    m_oceanFFT->setChoppy(m_params.choppy);

    // Create renderer
    m_renderer = std::make_unique<OceanRenderer>();
    if (!m_renderer->initialize(m_oceanFFT.get())) {
        std::cerr << "ERROR: Failed to initialize OceanRenderer\n";
        return false;
    }

    return true;
}

void Application::processInput() {
    if (!m_camera) return;

    bool speedBoost = glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                      glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

    // Camera movement
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera->processKeyboard(Camera::Direction::FORWARD, m_deltaTime, speedBoost);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera->processKeyboard(Camera::Direction::BACKWARD, m_deltaTime, speedBoost);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera->processKeyboard(Camera::Direction::LEFT, m_deltaTime, speedBoost);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera->processKeyboard(Camera::Direction::RIGHT, m_deltaTime, speedBoost);
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_camera->processKeyboard(Camera::Direction::UP, m_deltaTime, speedBoost);
    if (glfwGetKey(m_window, GLFW_KEY_C) == GLFW_PRESS)
        m_camera->processKeyboard(Camera::Direction::DOWN, m_deltaTime, speedBoost);
}

void Application::update() {
    if (!m_oceanFFT) return;

    m_frameCount++;
    
    // Update ocean simulation every 2 frames (optimisation)
    if (m_frameCount % 2 == 0) {
        m_oceanFFT->update(m_simTime);
    }

    // Apply parameter changes from UI
    if (std::abs(m_oceanFFT->getWindSpeed() - m_params.windSpeed) > 0.1f) {
        m_oceanFFT->setWindSpeed(m_params.windSpeed);
    }

    glm::vec2 windDir(m_params.windDirection[0], m_params.windDirection[1]);
    if (glm::length(m_oceanFFT->getWindDirection() - windDir) > 0.01f) {
        m_oceanFFT->setWindDirection(windDir);
    }

    if (std::abs(m_oceanFFT->getAmplitude() - m_params.amplitude) > 0.00001f) {
        m_oceanFFT->setAmplitude(m_params.amplitude);
    }

    if (std::abs(m_oceanFFT->getChoppy() - m_params.choppy) > 0.01f) {
        m_oceanFFT->setChoppy(m_params.choppy);
    }

    // Update renderer parameters
    if (m_renderer) {
        m_renderer->setWaterColor(glm::vec3(m_params.waterColor[0], 
                                            m_params.waterColor[1], 
                                            m_params.waterColor[2]));
        m_renderer->setFoamThreshold(m_params.foamThreshold);
        m_renderer->setWireframe(m_params.wireframe);
    }
}

void Application::render() {
    // Clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render ocean
    if (m_renderer && m_camera) {
        m_renderer->render(*m_camera, m_simTime);
    }

    // Render UI
    if (m_showUI) {
        renderUI();
    }
}

void Application::renderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Main control panel
    ImGui::Begin("Ocean FFT Simulator", &m_showUI);

    ImGui::Text("Controls:");
    ImGui::BulletText("WASD - Move | Mouse - Look");
    ImGui::BulletText("Shift - Speed | Space/C - Up/Down");
    ImGui::BulletText("Tab - Toggle Mouse | F1 - Toggle UI");
    ImGui::Separator();

    // Ocean parameters
    if (ImGui::CollapsingHeader("Ocean Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Wind Speed", &m_params.windSpeed, 5.0f, 60.0f, "%.1f m/s");
        ImGui::SliderFloat2("Wind Direction", m_params.windDirection, -1.0f, 1.0f);
        ImGui::SliderFloat("Amplitude", &m_params.amplitude, 0.00001f, 0.001f, "%.5f");
        ImGui::SliderFloat("Choppiness", &m_params.choppy, 0.0f, 5.0f, "%.2f");
        
        if (ImGui::Button("Calm Sea")) {
            m_params.windSpeed = 15.0f;
            m_params.amplitude = 0.0001f;
            m_params.choppy = 1.5f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Stormy Sea")) {
            m_params.windSpeed = 40.0f;
            m_params.amplitude = 0.0005f;
            m_params.choppy = 3.5f;
        }
    }

    // Rendering parameters
    if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::ColorEdit3("Water Color", m_params.waterColor);
        ImGui::SliderFloat("Foam Threshold", &m_params.foamThreshold, 0.0f, 2.0f);
        ImGui::Checkbox("Wireframe", &m_params.wireframe);
        ImGui::SliderFloat("Time Scale", &m_timeScale, 0.0f, 3.0f);
    }

    // Stats
    if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("FPS: %.1f (%.2f ms)", 1.0f / m_deltaTime, m_deltaTime * 1000.0f);
        ImGui::Text("Simulation Time: %.2f s", m_simTime);
        if (m_oceanFFT) {
            ImGui::Text("Resolution: %dx%d", m_oceanFFT->getResolution(), m_oceanFFT->getResolution());
            ImGui::Text("Patch Size: %.0f m", m_oceanFFT->getPatchSize());
        }
        if (m_camera) {
            glm::vec3 pos = m_camera->getPosition();
            ImGui::Text("Camera: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
        }
    }

    ImGui::End();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::cleanup() {
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup GLFW
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();

    std::cout << "Application cleaned up\n";
}

// GLFW callbacks

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->m_windowWidth = width;
        app->m_windowHeight = height;
    }
}

void Application::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app || !app->m_camera) return;
    
    // Si la souris n'est pas capturée OU si ImGui veut la souris, ne pas bouger la caméra
    ImGuiIO& io = ImGui::GetIO();
    if (!app->m_mouseCaptured || io.WantCaptureMouse) return;

    float xposf = static_cast<float>(xpos);
    float yposf = static_cast<float>(ypos);

    if (app->m_firstMouse) {
        app->m_lastMouseX = xposf;
        app->m_lastMouseY = yposf;
        app->m_firstMouse = false;
    }

    float xoffset = xposf - app->m_lastMouseX;
    float yoffset = app->m_lastMouseY - yposf; // Reversed: y-coordinates go from bottom to top

    app->m_lastMouseX = xposf;
    app->m_lastMouseY = yposf;

    app->m_camera->processMouseMovement(xoffset, yoffset);
}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app || !app->m_camera) return;

    app->m_camera->processMouseScroll(static_cast<float>(yoffset));
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app) return;

    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            
            case GLFW_KEY_TAB:
                app->m_mouseCaptured = !app->m_mouseCaptured;
                glfwSetInputMode(window, GLFW_CURSOR, 
                    app->m_mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                app->m_firstMouse = true;
                break;
            
            case GLFW_KEY_F1:
                app->m_showUI = !app->m_showUI;
                break;
        }
    }
}
