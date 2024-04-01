#include "Camera.h"

#include "../glm/include/glm/geometric.hpp"

#include <cmath>

Camera::Camera(int viewportWidth, int viewportHeight, glm::vec3 position, glm::vec3 target, glm::vec3 up, float verticalFovInRadians) noexcept :
    m_ViewportWidth(viewportWidth), m_ViewportHeight(viewportHeight), m_Position(position), m_Target(target), m_Up(up), m_VerticalFovInRadians(verticalFovInRadians) {
    ComputeRayDirections();
}

void Camera::OnViewportResize(int viewportWidth, int viewportHeight) noexcept {
    if (m_ViewportWidth == viewportWidth && m_ViewportHeight == viewportHeight) {
        return;
    }

    m_ViewportWidth = viewportWidth;
    m_ViewportHeight = viewportHeight;

    delete m_RayDirections;

    m_RayDirections = nullptr;

    ComputeRayDirections();
}

void Camera::ComputeRayDirections() noexcept {
    glm::vec3 forward = m_Target - m_Position;
    float focalLength = forward.length();
    float viewportWorldHeight = 2.f * tan(m_VerticalFovInRadians * 0.5f) * focalLength;
    float viewportWorldWidth = (viewportWorldHeight * m_ViewportWidth) / m_ViewportHeight;

    glm::vec3 w = glm::normalize(forward);
    glm::vec3 u = glm::normalize(glm::cross(w, m_Up));
    glm::vec3 v = glm::cross(w, u);

    glm::vec3 viewportHorizontal = u * (float)m_ViewportWidth;
    glm::vec3 viewportVertical = v * (float)m_ViewportHeight;
    glm::vec3 viewportLeftUpper = m_Position + forward - viewportHorizontal * 0.5f - viewportVertical * 0.5f;

    m_RayDirections = new glm::vec3[m_ViewportWidth * m_ViewportHeight];
    for (int i = 0; i < m_ViewportHeight; ++i) {
        for (int j = 0; j < m_ViewportWidth; ++j) {
            m_RayDirections[m_ViewportWidth * i + j] = viewportLeftUpper + viewportHorizontal * (float)j + viewportVertical * (float)i;
        }
    }
}

Camera::~Camera() noexcept {
    if (m_RayDirections != nullptr) {
        delete m_RayDirections;
    }
}