#include "Mesh.h"
#include <iostream>

Mesh::Mesh(int resolution, float size)
    : m_resolution(resolution)
    , m_size(size)
    , m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
    , m_indexCount(0) {
}

Mesh::~Mesh() {
    cleanup();
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_resolution(other.m_resolution)
    , m_size(other.m_size)
    , m_VAO(other.m_VAO)
    , m_VBO(other.m_VBO)
    , m_EBO(other.m_EBO)
    , m_indexCount(other.m_indexCount) {
    other.m_VAO = 0;
    other.m_VBO = 0;
    other.m_EBO = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        cleanup();
        m_resolution = other.m_resolution;
        m_size = other.m_size;
        m_VAO = other.m_VAO;
        m_VBO = other.m_VBO;
        m_EBO = other.m_EBO;
        m_indexCount = other.m_indexCount;
        other.m_VAO = 0;
        other.m_VBO = 0;
        other.m_EBO = 0;
    }
    return *this;
}

void Mesh::cleanup() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
    m_VAO = m_VBO = m_EBO = 0;
}

void Mesh::generate() {
    // Generate vertices
    std::vector<float> vertices;
    vertices.reserve(m_resolution * m_resolution * 5); // pos(3) + uv(2)

    float halfSize = m_size * 0.5f;
    float step = m_size / (m_resolution - 1);

    for (int z = 0; z < m_resolution; ++z) {
        for (int x = 0; x < m_resolution; ++x) {
            // Position (x, 0, z) - centered around origin
            float posX = -halfSize + x * step;
            float posZ = -halfSize + z * step;
            vertices.push_back(posX);
            vertices.push_back(0.0f);  // Y will be displaced by shader
            vertices.push_back(posZ);

            // Texture coordinates (0 to 1)
            float u = static_cast<float>(x) / (m_resolution - 1);
            float v = static_cast<float>(z) / (m_resolution - 1);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    // Generate indices (two triangles per quad)
    std::vector<unsigned int> indices;
    indices.reserve((m_resolution - 1) * (m_resolution - 1) * 6);

    for (int z = 0; z < m_resolution - 1; ++z) {
        for (int x = 0; x < m_resolution - 1; ++x) {
            unsigned int topLeft = z * m_resolution + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * m_resolution + x;
            unsigned int bottomRight = bottomLeft + 1;

            // First triangle (top-left, bottom-left, top-right)
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Second triangle (top-right, bottom-left, bottom-right)
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    m_indexCount = static_cast<int>(indices.size());

    // Create OpenGL buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Vertex attributes
    // Position (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Texture coordinates (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

    std::cout << "Mesh generated: " 
              << getVertexCount() << " vertices, " 
              << getTriangleCount() << " triangles\n";
}

void Mesh::render() const {
    if (m_VAO == 0) {
        std::cerr << "ERROR: Attempting to render mesh before generation\n";
        return;
    }

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
