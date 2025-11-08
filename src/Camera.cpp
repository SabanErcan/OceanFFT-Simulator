#include "Camera.h"
#include <algorithm>

Camera::Camera(const glm::vec3& position, const glm::vec3& worldUp)
    : m_position(position)
    , m_worldUp(worldUp)
    , m_yaw(DEFAULT_YAW)
    , m_pitch(DEFAULT_PITCH)
    , m_movementSpeed(DEFAULT_SPEED)
    , m_mouseSensitivity(DEFAULT_SENSITIVITY)
    , m_fov(DEFAULT_FOV)
    , m_nearPlane(0.1f)
    , m_farPlane(10000.0f) {
    updateCameraVectors();
}

void Camera::processKeyboard(Direction direction, float deltaTime, bool speedBoost) {
    float velocity = m_movementSpeed * deltaTime;
    if (speedBoost) {
        velocity *= SPEED_BOOST_MULTIPLIER;
    }

    switch (direction) {
        case Direction::FORWARD:
            m_position += m_front * velocity;
            break;
        case Direction::BACKWARD:
            m_position -= m_front * velocity;
            break;
        case Direction::LEFT:
            m_position -= m_right * velocity;
            break;
        case Direction::RIGHT:
            m_position += m_right * velocity;
            break;
        case Direction::UP:
            m_position += m_worldUp * velocity;
            break;
        case Direction::DOWN:
            m_position -= m_worldUp * velocity;
            break;
    }
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= m_mouseSensitivity;
    yOffset *= m_mouseSensitivity;

    m_yaw += xOffset;
    m_pitch += yOffset;

    // Constrain pitch to prevent screen flip
    if (constrainPitch) {
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    }

    // Normalize yaw to [0, 360)
    if (m_yaw > 360.0f) m_yaw -= 360.0f;
    if (m_yaw < 0.0f) m_yaw += 360.0f;

    updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset) {
    m_fov -= yOffset;
    m_fov = std::clamp(m_fov, MIN_FOV, MAX_FOV);
}

void Camera::setFOV(float fov) {
    m_fov = std::clamp(fov, MIN_FOV, MAX_FOV);
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::updateCameraVectors() {
    // Calculate new front vector from Euler angles
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    // Recalculate right and up vectors
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
