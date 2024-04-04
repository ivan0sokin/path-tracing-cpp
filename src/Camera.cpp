#include "Camera.h"
#include "Utilities.hpp"

#include "../glm/include/glm/geometric.hpp"

Camera::Camera(int viewportWidth, int viewportHeight, glm::vec3 position, glm::vec3 target, glm::vec3 up, float verticalFovInRadians) noexcept :
    m_ViewportWidth(viewportWidth), m_ViewportHeight(viewportHeight), m_Position(position), m_Target(target), m_Up(up), m_VerticalFovInRadians(verticalFovInRadians) {
    m_RayDirections = new glm::vec3[m_ViewportWidth * m_ViewportHeight];
    ComputeRayDirections();
}

void Camera::OnViewportResize(int viewportWidth, int viewportHeight) noexcept {
    if (m_ViewportWidth == viewportWidth && m_ViewportHeight == viewportHeight) {
        return;
    }

    m_ViewportWidth = viewportWidth;
    m_ViewportHeight = viewportHeight;

    if (m_RayDirections != nullptr) {
        delete m_RayDirections;
    }
    m_RayDirections = new glm::vec3[m_ViewportWidth * m_ViewportHeight];

    ComputeRayDirections();
}

#include "stdio.h"

void Camera::ComputeRayDirections() noexcept {
    glm::vec3 forward = m_Target - m_Position;
    float focalLength = forward.length();
    float viewportWorldHeight = 2.f * tan(m_VerticalFovInRadians * 0.5f) * focalLength;
    float viewportWorldWidth = (viewportWorldHeight * m_ViewportWidth) / m_ViewportHeight;

    glm::vec3 w = glm::normalize(forward);  
    glm::vec3 u = glm::normalize(glm::cross(w, m_Up));
    glm::vec3 v = glm::cross(w, u);

    glm::vec3 horizontal = u * viewportWorldWidth;
    glm::vec3 vertical = v * viewportWorldHeight;
    glm::vec3 leftUpper = m_Position + forward - horizontal * 0.5f - vertical * 0.5f; 

    for (int i = 0; i < m_ViewportHeight; ++i) {
        for (int j = 0; j < m_ViewportWidth; ++j) {
            float uScale = (float)(j + Utilities::RandomFloatInNegativeHalfToHalf()) / (m_ViewportWidth - 1);
            float vScale = (float)(i + Utilities::RandomFloatInNegativeHalfToHalf()) / (m_ViewportHeight - 1);
            m_RayDirections[m_ViewportWidth * i + j] = glm::normalize(leftUpper + horizontal * uScale + vertical * vScale);
        }
    }
}

Camera::~Camera() noexcept {
    if (m_RayDirections != nullptr) {
        delete m_RayDirections;
    }
}