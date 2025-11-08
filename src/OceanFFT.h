#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <complex>
#include <vector>
#include <fftw3.h>

/**
 * @brief FFT-based ocean wave simulation using Phillips spectrum
 * 
 * Implements Tessendorf's FFT ocean simulation:
 * 1. Generates initial spectrum h0(k) using Phillips spectrum
 * 2. Evolves spectrum over time: h(k,t) = h0(k)*exp(iωt) + h0*(-k)*exp(-iωt)
 * 3. Performs inverse FFT to get spatial domain (height field)
 * 4. Calculates normals and choppy displacement
 * 5. Uploads to GPU as textures
 */
class OceanFFT {
public:
    /**
     * @brief Create ocean simulation
     * @param N Resolution (power of 2, e.g., 256 or 512)
     * @param L Physical patch size in meters (e.g., 1000.0)
     */
    OceanFFT(int N, float L);
    ~OceanFFT();

    // Non-copyable
    OceanFFT(const OceanFFT&) = delete;
    OceanFFT& operator=(const OceanFFT&) = delete;

    /**
     * @brief Initialize FFT plans and generate initial spectrum
     */
    bool initialize();

    /**
     * @brief Update simulation for given time
     * @param time Simulation time in seconds
     */
    void update(float time);

    // Parameter setters (regenerate h0 on change)
    void setWindSpeed(float speed);
    void setWindDirection(const glm::vec2& direction);
    void setAmplitude(float amplitude);
    void setChoppy(float choppy);

    // Getters
    GLuint getDisplacementTexture() const { return m_texDisplacement; }
    GLuint getNormalTexture() const { return m_texNormal; }
    int getResolution() const { return m_N; }
    float getPatchSize() const { return m_L; }
    float getWindSpeed() const { return m_windSpeed; }
    glm::vec2 getWindDirection() const { return m_windDirection; }
    float getAmplitude() const { return m_amplitude; }
    float getChoppy() const { return m_choppy; }

private:
    // Simulation parameters
    int m_N;                    // Resolution (e.g., 256)
    float m_L;                  // Patch size in meters
    float m_windSpeed;          // Wind speed in m/s
    glm::vec2 m_windDirection;  // Normalized wind direction
    float m_amplitude;          // Wave amplitude multiplier (A)
    float m_choppy;             // Choppiness factor

    // Physics constants
    static constexpr float GRAVITY = 9.81f;  // m/s²

    // FFTW data structures
    fftwf_plan m_planHeight;     // FFT plan for height displacement
    fftwf_plan m_planChoppyX;    // FFT plan for X choppy displacement
    fftwf_plan m_planChoppyZ;    // FFT plan for Z choppy displacement
    fftwf_plan m_planNormalX;    // FFT plan for normal X component
    fftwf_plan m_planNormalZ;    // FFT plan for normal Z component

    // Spectrum data (frequency domain)
    std::vector<std::complex<float>> m_h0;          // Initial spectrum h0(k)
    std::vector<std::complex<float>> m_h0Conj;      // Conjugate h0*(-k)
    std::vector<std::complex<float>> m_htilde;      // Time-evolved h(k,t)
    std::vector<std::complex<float>> m_htildeChoppyX;
    std::vector<std::complex<float>> m_htildeChoppyZ;
    std::vector<std::complex<float>> m_htildeNormalX;
    std::vector<std::complex<float>> m_htildeNormalZ;

    // Spatial domain data (output of FFT)
    std::vector<float> m_heightField;    // Y displacement
    std::vector<float> m_choppyX;        // X displacement
    std::vector<float> m_choppyZ;        // Z displacement
    std::vector<float> m_normalX;        // Normal X component
    std::vector<float> m_normalZ;        // Normal Z component

    // OpenGL textures
    GLuint m_texDisplacement;    // RGB = (dx, dy, dz)
    GLuint m_texNormal;          // RGB = (nx, ny, nz)

    // Helper methods

    /**
     * @brief Generate initial spectrum h0(k) using Phillips spectrum
     */
    void generateH0();

    /**
     * @brief Evaluate wave spectrum at given time
     * @param t Time in seconds
     */
    void evaluateWaves(float t);

    /**
     * @brief Execute FFT transforms
     */
    void executeFFT();

    /**
     * @brief Update OpenGL textures with FFT results
     */
    void updateTextures();

    /**
     * @brief Phillips spectrum function
     * @param k Wave vector
     * @return Spectrum amplitude
     */
    float phillipsSpectrum(const glm::vec2& k) const;

    /**
     * @brief Dispersion relation: ω(k) = sqrt(g|k|)
     * @param k Wave vector
     * @return Angular frequency
     */
    float dispersion(const glm::vec2& k) const;

    /**
     * @brief Generate Gaussian random number (Box-Muller)
     * @return Random value from N(0,1)
     */
    float gaussianRandom() const;

    /**
     * @brief Get wave vector k for grid position (x, z)
     */
    glm::vec2 getWaveVector(int x, int z) const;

    /**
     * @brief Get array index for grid position (x, z)
     */
    int getIndex(int x, int z) const;

    /**
     * @brief Create OpenGL textures
     */
    void createTextures();

    /**
     * @brief Clean up FFTW resources
     */
    void cleanupFFTW();
};
