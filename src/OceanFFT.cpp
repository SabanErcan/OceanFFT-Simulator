#include "OceanFFT.h"
#include <iostream>
#include <cmath>
#include <random>

OceanFFT::OceanFFT(int N, float L)
    : m_N(N)
    , m_L(L)
    , m_windSpeed(30.0f)
    , m_windDirection(1.0f, 0.0f)
    , m_amplitude(0.0002f)
    , m_choppy(2.0f)
    , m_planHeight(nullptr)
    , m_planChoppyX(nullptr)
    , m_planChoppyZ(nullptr)
    , m_planNormalX(nullptr)
    , m_planNormalZ(nullptr)
    , m_texDisplacement(0)
    , m_texNormal(0) {
    
    // Allocate memory
    int size = m_N * m_N;
    m_h0.resize(size);
    m_h0Conj.resize(size);
    m_htilde.resize(size);
    m_htildeChoppyX.resize(size);
    m_htildeChoppyZ.resize(size);
    m_htildeNormalX.resize(size);
    m_htildeNormalZ.resize(size);
    m_heightField.resize(size);
    m_choppyX.resize(size);
    m_choppyZ.resize(size);
    m_normalX.resize(size);
    m_normalZ.resize(size);
}

OceanFFT::~OceanFFT() {
    cleanupFFTW();
    
    if (m_texDisplacement) glDeleteTextures(1, &m_texDisplacement);
    if (m_texNormal) glDeleteTextures(1, &m_texNormal);
}

bool OceanFFT::initialize() {
    std::cout << "Initializing OceanFFT (N=" << m_N << ", L=" << m_L << "m)...\n";

    // Generate initial spectrum
    generateH0();

    // Create FFTW plans (using FFTW_ESTIMATE for faster planning)
    // Plans convert frequency domain (complex) to spatial domain (real)
    m_planHeight = fftwf_plan_dft_c2r_2d(
        m_N, m_N,
        reinterpret_cast<fftwf_complex*>(m_htilde.data()),
        m_heightField.data(),
        FFTW_ESTIMATE
    );

    m_planChoppyX = fftwf_plan_dft_c2r_2d(
        m_N, m_N,
        reinterpret_cast<fftwf_complex*>(m_htildeChoppyX.data()),
        m_choppyX.data(),
        FFTW_ESTIMATE
    );

    m_planChoppyZ = fftwf_plan_dft_c2r_2d(
        m_N, m_N,
        reinterpret_cast<fftwf_complex*>(m_htildeChoppyZ.data()),
        m_choppyZ.data(),
        FFTW_ESTIMATE
    );

    m_planNormalX = fftwf_plan_dft_c2r_2d(
        m_N, m_N,
        reinterpret_cast<fftwf_complex*>(m_htildeNormalX.data()),
        m_normalX.data(),
        FFTW_ESTIMATE
    );

    m_planNormalZ = fftwf_plan_dft_c2r_2d(
        m_N, m_N,
        reinterpret_cast<fftwf_complex*>(m_htildeNormalZ.data()),
        m_normalZ.data(),
        FFTW_ESTIMATE
    );

    if (!m_planHeight || !m_planChoppyX || !m_planChoppyZ || 
        !m_planNormalX || !m_planNormalZ) {
        std::cerr << "ERROR: Failed to create FFTW plans\n";
        return false;
    }

    // Create OpenGL textures
    createTextures();

    std::cout << "OceanFFT initialized successfully\n";
    return true;
}

void OceanFFT::update(float time) {
    // Evaluate spectrum at current time
    evaluateWaves(time);

    // Execute FFT transforms
    executeFFT();

    // Upload to GPU
    updateTextures();
}

void OceanFFT::setWindSpeed(float speed) {
    if (std::abs(m_windSpeed - speed) > 0.01f) {
        m_windSpeed = speed;
        generateH0();
    }
}

void OceanFFT::setWindDirection(const glm::vec2& direction) {
    glm::vec2 normalized = glm::normalize(direction);
    if (glm::length(m_windDirection - normalized) > 0.01f) {
        m_windDirection = normalized;
        generateH0();
    }
}

void OceanFFT::setAmplitude(float amplitude) {
    if (std::abs(m_amplitude - amplitude) > 0.00001f) {
        m_amplitude = amplitude;
        generateH0();
    }
}

void OceanFFT::setChoppy(float choppy) {
    m_choppy = choppy;
}

void OceanFFT::generateH0() {
    std::cout << "Generating h0 spectrum (wind: " << m_windSpeed 
              << "m/s, amplitude: " << m_amplitude << ")...\n";

    for (int z = 0; z < m_N; ++z) {
        for (int x = 0; x < m_N; ++x) {
            glm::vec2 k = getWaveVector(x, z);
            int idx = getIndex(x, z);

            // Phillips spectrum
            float Ph = phillipsSpectrum(k);

            // Generate complex Gaussian random variables
            float xi_r = gaussianRandom();
            float xi_i = gaussianRandom();

            // h0(k) = 1/sqrt(2) * (xi_r + i*xi_i) * sqrt(P(k))
            float sqrtPh = std::sqrt(Ph);
            m_h0[idx] = std::complex<float>(xi_r, xi_i) * sqrtPh * 0.707106781f; // 1/sqrt(2)

            // h0*(-k) for conjugate symmetry
            glm::vec2 kNeg = -k;
            float PhNeg = phillipsSpectrum(kNeg);
            float xi_r_neg = gaussianRandom();
            float xi_i_neg = gaussianRandom();
            float sqrtPhNeg = std::sqrt(PhNeg);
            m_h0Conj[idx] = std::conj(
                std::complex<float>(xi_r_neg, xi_i_neg) * sqrtPhNeg * 0.707106781f
            );
        }
    }
}

void OceanFFT::evaluateWaves(float t) {
    using namespace std::complex_literals;

    for (int z = 0; z < m_N; ++z) {
        for (int x = 0; x < m_N; ++x) {
            int idx = getIndex(x, z);
            glm::vec2 k = getWaveVector(x, z);
            float kLen = glm::length(k);

            // Dispersion relation: ω(k) = sqrt(g|k|)
            float omega = dispersion(k);

            // Time evolution: h(k,t) = h0(k)*exp(iωt) + h0*(-k)*exp(-iωt)
            std::complex<float> expIwt = std::exp(1if * omega * t);
            std::complex<float> expMinusIwt = std::conj(expIwt);

            m_htilde[idx] = m_h0[idx] * expIwt + m_h0Conj[idx] * expMinusIwt;

            // Choppy displacement: D(x) = -i * k/|k| * h(k,t)
            if (kLen > 0.0001f) {
                std::complex<float> factor = -1if * m_htilde[idx] / kLen;
                m_htildeChoppyX[idx] = factor * k.x;
                m_htildeChoppyZ[idx] = factor * k.y;
            } else {
                m_htildeChoppyX[idx] = 0.0f;
                m_htildeChoppyZ[idx] = 0.0f;
            }

            // Normal calculation: N = (-∂h/∂x, 1, -∂h/∂z)
            // In frequency domain: ∂h/∂x ↔ i*kx*h(k), ∂h/∂z ↔ i*kz*h(k)
            m_htildeNormalX[idx] = 1if * k.x * m_htilde[idx];
            m_htildeNormalZ[idx] = 1if * k.y * m_htilde[idx];
        }
    }
}

void OceanFFT::executeFFT() {
    // Execute inverse FFT transforms
    fftwf_execute(m_planHeight);
    fftwf_execute(m_planChoppyX);
    fftwf_execute(m_planChoppyZ);
    fftwf_execute(m_planNormalX);
    fftwf_execute(m_planNormalZ);

    // Normalize (FFTW doesn't normalize inverse transforms)
    float norm = 1.0f / (m_N * m_N);
    for (int i = 0; i < m_N * m_N; ++i) {
        m_heightField[i] *= norm;
        m_choppyX[i] *= norm * m_choppy;
        m_choppyZ[i] *= norm * m_choppy;
        m_normalX[i] *= norm;
        m_normalZ[i] *= norm;
    }
}

void OceanFFT::updateTextures() {
    // Prepare displacement texture data (RGB = dx, dy, dz)
    std::vector<float> displacementData(m_N * m_N * 3);
    std::vector<float> normalData(m_N * m_N * 3);

    for (int z = 0; z < m_N; ++z) {
        for (int x = 0; x < m_N; ++x) {
            int idx = getIndex(x, z);
            int texIdx = (z * m_N + x) * 3;

            // Displacement (x, y, z)
            displacementData[texIdx + 0] = m_choppyX[idx];
            displacementData[texIdx + 1] = m_heightField[idx];
            displacementData[texIdx + 2] = m_choppyZ[idx];

            // Normal (-∂h/∂x, 1, -∂h/∂z) normalized
            glm::vec3 normal(-m_normalX[idx], 1.0f, -m_normalZ[idx]);
            normal = glm::normalize(normal);
            normalData[texIdx + 0] = normal.x;
            normalData[texIdx + 1] = normal.y;
            normalData[texIdx + 2] = normal.z;
        }
    }

    // Upload to GPU
    glBindTexture(GL_TEXTURE_2D, m_texDisplacement);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_N, m_N, 
                    GL_RGB, GL_FLOAT, displacementData.data());

    glBindTexture(GL_TEXTURE_2D, m_texNormal);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_N, m_N, 
                    GL_RGB, GL_FLOAT, normalData.data());

    glBindTexture(GL_TEXTURE_2D, 0);
}

float OceanFFT::phillipsSpectrum(const glm::vec2& k) const {
    float kLen = glm::length(k);
    if (kLen < 0.0001f) return 0.0f;

    // L = V² / g (largest possible wave from wind speed V)
    float L = (m_windSpeed * m_windSpeed) / GRAVITY;

    // Alignment with wind direction
    glm::vec2 kNorm = k / kLen;
    float kDotW = glm::dot(kNorm, m_windDirection);
    float kDotW2 = kDotW * kDotW;

    // Suppress waves smaller than cutoff
    float l = L / 1000.0f;  // Small wave cutoff

    // Phillips spectrum formula:
    // P(k) = A * exp(-1/(kL)²) / k⁴ * (k̂·ŵ)² * exp(-k²l²)
    float kLen2 = kLen * kLen;
    float kLen4 = kLen2 * kLen2;

    float Ph = m_amplitude 
             * std::exp(-1.0f / (kLen2 * L * L))
             / kLen4
             * kDotW2
             * std::exp(-kLen2 * l * l);

    return Ph;
}

float OceanFFT::dispersion(const glm::vec2& k) const {
    float kLen = glm::length(k);
    return std::sqrt(GRAVITY * kLen);
}

float OceanFFT::gaussianRandom() const {
    // Thread-local random generator
    static thread_local std::mt19937 generator(std::random_device{}());
    static thread_local std::normal_distribution<float> distribution(0.0f, 1.0f);
    return distribution(generator);
}

glm::vec2 OceanFFT::getWaveVector(int x, int z) const {
    // k = 2π * (n - N/2) / L, where n ∈ [0, N)
    const float PI = 3.14159265358979323846f;
    float kx = (2.0f * PI * (x - m_N / 2.0f)) / m_L;
    float kz = (2.0f * PI * (z - m_N / 2.0f)) / m_L;
    return glm::vec2(kx, kz);
}

int OceanFFT::getIndex(int x, int z) const {
    return z * m_N + x;
}

void OceanFFT::createTextures() {
    // Displacement texture (RGB32F)
    glGenTextures(1, &m_texDisplacement);
    glBindTexture(GL_TEXTURE_2D, m_texDisplacement);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_N, m_N, 0, 
                 GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Normal texture (RGB32F)
    glGenTextures(1, &m_texNormal);
    glBindTexture(GL_TEXTURE_2D, m_texNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_N, m_N, 0, 
                 GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Created displacement and normal textures\n";
}

void OceanFFT::cleanupFFTW() {
    if (m_planHeight) fftwf_destroy_plan(m_planHeight);
    if (m_planChoppyX) fftwf_destroy_plan(m_planChoppyX);
    if (m_planChoppyZ) fftwf_destroy_plan(m_planChoppyZ);
    if (m_planNormalX) fftwf_destroy_plan(m_planNormalX);
    if (m_planNormalZ) fftwf_destroy_plan(m_planNormalZ);
    
    m_planHeight = nullptr;
    m_planChoppyX = nullptr;
    m_planChoppyZ = nullptr;
    m_planNormalX = nullptr;
    m_planNormalZ = nullptr;
}
