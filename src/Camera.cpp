#include "Camera.h"
#include "Utilities.hpp"

Camera::Camera(int viewportWidth, int viewportHeight, const Math::Vector3f &position, const Math::Vector3f &target, float verticalFovInRadians, const Math::Vector3f &up) noexcept :
    m_ViewportWidth(viewportWidth), m_ViewportHeight(viewportHeight), m_Position(position), m_Target(target), m_VerticalFovInRadians(verticalFovInRadians), m_Up(up) {
    m_RayDirections = new Math::Vector3f[m_ViewportWidth * m_ViewportHeight];
    ComputeRayDirections();
}

void Camera::OnViewportResize(int viewportWidth, int viewportHeight) noexcept {
    if (m_ViewportWidth == viewportWidth && m_ViewportHeight == viewportHeight) {
        return;
    }

    m_ViewportWidth = viewportWidth;
    m_ViewportHeight = viewportHeight;

    if (m_RayDirections != nullptr) {
        delete[] m_RayDirections;
    }
    m_RayDirections = new Math::Vector3f[m_ViewportWidth * m_ViewportHeight];

    ComputeRayDirections();
}

void Camera::ComputeRayDirections() noexcept {
    Math::Vector3f forward = m_Target - m_Position;
    float focalLength = Math::Length(forward);
    float viewportWorldHeight = 2.f * Math::Tan(m_VerticalFovInRadians * 0.5f) * focalLength;
    float viewportWorldWidth = (viewportWorldHeight * m_ViewportWidth) / m_ViewportHeight;

    Math::Vector3f w = Math::Normalize(forward);  
    Math::Vector3f u = Math::Normalize(Math::Cross(w, m_Up));
    Math::Vector3f v = Math::Normalize(Math::Cross(w, u));

    Math::Vector3f horizontal = u * viewportWorldWidth;
    Math::Vector3f vertical = v * viewportWorldHeight;
    Math::Vector3f leftUpper = forward - horizontal * 0.5f - vertical * 0.5f;

    for (int i = 0; i < m_ViewportHeight; ++i) {
        for (int j = 0; j < m_ViewportWidth; ++j) {
            float uScale = (float)(j + Utilities::RandomFloatInNegativeHalfToHalf()) / (m_ViewportWidth - 1);
            float vScale = (float)(i + Utilities::RandomFloatInNegativeHalfToHalf()) / (m_ViewportHeight - 1);
            m_RayDirections[m_ViewportWidth * i + j] = Math::Normalize(leftUpper + horizontal * uScale + vertical * vScale);
        }
    }
}

Camera::~Camera() noexcept {
    if (m_RayDirections != nullptr) {
        delete[] m_RayDirections;
    }
}