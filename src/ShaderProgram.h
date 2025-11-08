#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

/**
 * @brief Manages OpenGL shader programs
 * 
 * Handles shader compilation, linking, and uniform management with caching
 */
class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    // Non-copyable but movable
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&&) noexcept;
    ShaderProgram& operator=(ShaderProgram&&) noexcept;

    /**
     * @brief Load and compile shaders from files
     * @param vertexPath Path to vertex shader
     * @param fragmentPath Path to fragment shader
     * @return true if successful, false otherwise
     */
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * @brief Activate this shader program
     */
    void use() const;

    /**
     * @brief Check if shader program is valid
     */
    bool isValid() const { return m_programID != 0; }

    /**
     * @brief Get OpenGL program ID
     */
    GLuint getID() const { return m_programID; }

    // Uniform setters (cached for performance)
    void setUniform(const std::string& name, bool value);
    void setUniform(const std::string& name, int value);
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, const glm::vec2& value);
    void setUniform(const std::string& name, const glm::vec3& value);
    void setUniform(const std::string& name, const glm::vec4& value);
    void setUniform(const std::string& name, const glm::mat3& value);
    void setUniform(const std::string& name, const glm::mat4& value);

private:
    GLuint m_programID;
    std::unordered_map<std::string, GLint> m_uniformCache;

    /**
     * @brief Get uniform location (cached)
     */
    GLint getUniformLocation(const std::string& name);

    /**
     * @brief Read shader source from file
     */
    std::string readFile(const std::string& filepath);

    /**
     * @brief Compile a shader
     * @param source Shader source code
     * @param type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
     * @return Shader ID or 0 on failure
     */
    GLuint compileShader(const std::string& source, GLenum type);

    /**
     * @brief Link shaders into program
     * @param vertexShader Compiled vertex shader ID
     * @param fragmentShader Compiled fragment shader ID
     * @return Program ID or 0 on failure
     */
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);

    /**
     * @brief Check shader compilation errors
     */
    bool checkCompileErrors(GLuint shader, const std::string& type);

    /**
     * @brief Check program linking errors
     */
    bool checkLinkErrors(GLuint program);
};
