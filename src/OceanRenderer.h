#pragma once

#include "OceanFFT.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include <memory>

/**
 * @brief Renders the FFT ocean using OpenGL
 * 
 * Manages ocean mesh, shaders, and rendering state
 */
class OceanRenderer {
public:
    OceanRenderer();
    ~OceanRenderer() = default;

    /**
     * @brief Initialize renderer with ocean simulation
     * @param oceanFFT Ocean simulation instance
     * @return true if successful
     */
    bool initialize(OceanFFT* oceanFFT);

    /**
     * @brief Render the ocean
     * @param camera Camera for view/projection matrices
     * @param time Current simulation time
     */
    void render(const Camera& camera, float time);

    /**
     * @brief Toggle wireframe mode
     */
    void setWireframe(bool enabled) { m_wireframe = enabled; }
    bool isWireframe() const { return m_wireframe; }

    /**
     * @brief Set rendering parameters
     */
    void setWaterColor(const glm::vec3& color) { m_waterColor = color; }
    void setFoamThreshold(float threshold) { m_foamThreshold = threshold; }
    void setSunDirection(const glm::vec3& direction) { m_sunDirection = glm::normalize(direction); }

    /**
     * @brief Get rendering parameters
     */
    const glm::vec3& getWaterColor() const { return m_waterColor; }
    float getFoamThreshold() const { return m_foamThreshold; }
    const glm::vec3& getSunDirection() const { return m_sunDirection; }

private:
    OceanFFT* m_oceanFFT;
    std::unique_ptr<Mesh> m_mesh;
    std::unique_ptr<ShaderProgram> m_shader;

    // Rendering parameters
    bool m_wireframe;
    glm::vec3 m_waterColor;
    float m_foamThreshold;
    glm::vec3 m_sunDirection;

    // Skybox (simplified - single color for now)
    glm::vec3 m_skyColor;
};
