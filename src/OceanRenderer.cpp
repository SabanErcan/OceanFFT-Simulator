#include "OceanRenderer.h"
#include <iostream>

OceanRenderer::OceanRenderer()
    : m_oceanFFT(nullptr)
    , m_wireframe(false)
    , m_waterColor(0.0f, 0.3f, 0.5f)
    , m_foamThreshold(0.5f)
    , m_sunDirection(1.0f, 1.0f, 0.5f)
    , m_skyColor(0.5f, 0.7f, 1.0f) {
    m_sunDirection = glm::normalize(m_sunDirection);
}

bool OceanRenderer::initialize(OceanFFT* oceanFFT) {
    if (!oceanFFT) {
        std::cerr << "ERROR: Ocean FFT is null\n";
        return false;
    }

    m_oceanFFT = oceanFFT;

    // Create mesh with reduced resolution for better performance
    // Using half the FFT resolution (128/2 = 64) for rendering
    int meshRes = oceanFFT->getResolution() / 2;
    m_mesh = std::make_unique<Mesh>(meshRes, oceanFFT->getPatchSize());
    m_mesh->generate();

    // Load ocean shader
    m_shader = std::make_unique<ShaderProgram>();
    if (!m_shader->loadFromFiles("shaders/ocean.vert", "shaders/ocean.frag")) {
        std::cerr << "ERROR: Failed to load ocean shader\n";
        return false;
    }

    std::cout << "OceanRenderer initialized\n";
    return true;
}

void OceanRenderer::render(const Camera& camera, float time) {
    if (!m_oceanFFT || !m_mesh || !m_shader || !m_shader->isValid()) {
        return;
    }

    // Set wireframe mode
    if (m_wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Use shader
    m_shader->use();

    // Set matrices
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(16.0f / 9.0f); // TODO: Use actual aspect ratio

    m_shader->setUniform("uModel", model);
    m_shader->setUniform("uView", view);
    m_shader->setUniform("uProj", projection);

    // Set camera position
    m_shader->setUniform("uCameraPos", camera.getPosition());

    // Bind displacement and normal textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_oceanFFT->getDisplacementTexture());
    m_shader->setUniform("uDisplacement", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_oceanFFT->getNormalTexture());
    m_shader->setUniform("uNormals", 1);

    // Set rendering parameters
    m_shader->setUniform("uWaterColor", m_waterColor);
    m_shader->setUniform("uFoamThreshold", m_foamThreshold);
    m_shader->setUniform("uSunDirection", m_sunDirection);
    m_shader->setUniform("uSkyColor", m_skyColor);
    m_shader->setUniform("uTime", time);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Render mesh
    m_mesh->render();

    // Cleanup
    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
