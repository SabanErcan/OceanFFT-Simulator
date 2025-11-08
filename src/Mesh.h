#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

/**
 * @brief Generates and manages a grid mesh for ocean rendering
 * 
 * Creates a NxN vertex grid with indexed triangles.
 * Topology is static, only positions/normals are updated per frame.
 */
class Mesh {
public:
    /**
     * @brief Create a grid mesh
     * @param resolution Number of vertices per side (e.g., 256)
     * @param size Physical size in world units (e.g., 1000.0f meters)
     */
    Mesh(int resolution, float size);
    ~Mesh();

    // Non-copyable but movable
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) noexcept;
    Mesh& operator=(Mesh&&) noexcept;

    /**
     * @brief Generate the grid mesh
     */
    void generate();

    /**
     * @brief Render the mesh
     */
    void render() const;

    /**
     * @brief Get mesh info
     */
    int getResolution() const { return m_resolution; }
    int getVertexCount() const { return m_resolution * m_resolution; }
    int getTriangleCount() const { return (m_resolution - 1) * (m_resolution - 1) * 2; }
    float getSize() const { return m_size; }

private:
    int m_resolution;        // Vertices per side (N)
    float m_size;            // Physical size in world units
    
    GLuint m_VAO;            // Vertex Array Object
    GLuint m_VBO;            // Vertex Buffer Object
    GLuint m_EBO;            // Element Buffer Object
    
    int m_indexCount;        // Number of indices

    void cleanup();
};
