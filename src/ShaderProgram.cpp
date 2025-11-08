#include "ShaderProgram.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram() : m_programID(0) {}

ShaderProgram::~ShaderProgram() {
    if (m_programID != 0) {
        glDeleteProgram(m_programID);
    }
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : m_programID(other.m_programID)
    , m_uniformCache(std::move(other.m_uniformCache)) {
    other.m_programID = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
    if (this != &other) {
        if (m_programID != 0) {
            glDeleteProgram(m_programID);
        }
        m_programID = other.m_programID;
        m_uniformCache = std::move(other.m_uniformCache);
        other.m_programID = 0;
    }
    return *this;
}

bool ShaderProgram::loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    // Read shader sources
    std::string vertexSource = readFile(vertexPath);
    std::string fragmentSource = readFile(fragmentPath);

    if (vertexSource.empty() || fragmentSource.empty()) {
        std::cerr << "ERROR: Failed to read shader files\n";
        return false;
    }

    // Compile shaders
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        std::cerr << "ERROR: Failed to compile vertex shader: " << vertexPath << "\n";
        return false;
    }

    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        std::cerr << "ERROR: Failed to compile fragment shader: " << fragmentPath << "\n";
        glDeleteShader(vertexShader);
        return false;
    }

    // Link program
    m_programID = linkProgram(vertexShader, fragmentShader);

    // Clean up shaders (no longer needed after linking)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (m_programID == 0) {
        std::cerr << "ERROR: Failed to link shader program\n";
        return false;
    }

    std::cout << "Successfully loaded shader: " << vertexPath << " + " << fragmentPath << "\n";
    return true;
}

void ShaderProgram::use() const {
    if (m_programID != 0) {
        glUseProgram(m_programID);
    }
}

std::string ShaderProgram::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file: " << filepath << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint ShaderProgram::compileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    if (!checkCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint ShaderProgram::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    if (!checkLinkErrors(program)) {
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

bool ShaderProgram::checkCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "ERROR: Shader compilation failed (" << type << ")\n" << infoLog << "\n";
        return false;
    }
    return true;
}

bool ShaderProgram::checkLinkErrors(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    
    if (!success) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cerr << "ERROR: Program linking failed\n" << infoLog << "\n";
        return false;
    }
    return true;
}

GLint ShaderProgram::getUniformLocation(const std::string& name) {
    // Check cache first
    auto it = m_uniformCache.find(name);
    if (it != m_uniformCache.end()) {
        return it->second;
    }

    // Query OpenGL and cache result
    GLint location = glGetUniformLocation(m_programID, name.c_str());
    m_uniformCache[name] = location;

    if (location == -1) {
        std::cerr << "WARNING: Uniform '" << name << "' not found in shader\n";
    }

    return location;
}

// Uniform setters implementations
void ShaderProgram::setUniform(const std::string& name, bool value) {
    glUniform1i(getUniformLocation(name), static_cast<int>(value));
}

void ShaderProgram::setUniform(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void ShaderProgram::setUniform(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec2& value) {
    glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& value) {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4& value) {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat3& value) {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
