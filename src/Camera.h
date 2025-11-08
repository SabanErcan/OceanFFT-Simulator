#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief FPS-style camera with WASD movement and mouse look
 * 
 * Supports:
 * - WASD movement in camera space
 * - Mouse look (yaw/pitch)
 * - Speed boost (Shift)
 * - Pitch clamping to prevent gimbal lock
 */
class Camera {
public:
    /**
     * @brief Create camera at position
     * @param position Initial position
     * @param worldUp World up vector (default: +Y)
     */
    Camera(const glm::vec3& position = glm::vec3(0.0f, 50.0f, 100.0f),
           const glm::vec3& worldUp = glm::vec3(0.0f, 1.0f, 0.0f));

    // Movement
    enum class Direction {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    /**
     * @brief Process keyboard input for movement
     * @param direction Movement direction
     * @param deltaTime Time since last frame
     * @param speedBoost Apply speed multiplier (Shift key)
     */
    void processKeyboard(Direction direction, float deltaTime, bool speedBoost = false);

    /**
     * @brief Process mouse movement for rotation
     * @param xOffset Mouse X offset
     * @param yOffset Mouse Y offset
     * @param constrainPitch Clamp pitch to [-89, 89] degrees
     */
    void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

    /**
     * @brief Process mouse scroll for FOV zoom
     * @param yOffset Scroll offset
     */
    void processMouseScroll(float yOffset);

    /**
     * @brief Get view matrix for rendering
     */
    glm::mat4 getViewMatrix() const;

    /**
     * @brief Get projection matrix
     * @param aspectRatio Window aspect ratio (width/height)
     */
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    // Getters
    const glm::vec3& getPosition() const { return m_position; }
    const glm::vec3& getFront() const { return m_front; }
    const glm::vec3& getUp() const { return m_up; }
    const glm::vec3& getRight() const { return m_right; }
    float getYaw() const { return m_yaw; }
    float getPitch() const { return m_pitch; }
    float getFOV() const { return m_fov; }

    // Setters
    void setPosition(const glm::vec3& position) { m_position = position; }
    void setMovementSpeed(float speed) { m_movementSpeed = speed; }
    void setMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }
    void setFOV(float fov);

private:
    // Camera attributes
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    // Euler angles
    float m_yaw;          // Left/right rotation (degrees)
    float m_pitch;        // Up/down rotation (degrees)

    // Camera options
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_fov;          // Field of view (degrees)
    float m_nearPlane;
    float m_farPlane;

    // Constants
    static constexpr float DEFAULT_YAW = -90.0f;
    static constexpr float DEFAULT_PITCH = 0.0f;
    static constexpr float DEFAULT_SPEED = 50.0f;
    static constexpr float DEFAULT_SENSITIVITY = 0.015f;  // Sensibilité très réduite
    static constexpr float DEFAULT_FOV = 45.0f;
    static constexpr float MIN_FOV = 1.0f;
    static constexpr float MAX_FOV = 90.0f;
    static constexpr float SPEED_BOOST_MULTIPLIER = 3.0f;

    /**
     * @brief Update camera vectors from Euler angles
     */
    void updateCameraVectors();
};
