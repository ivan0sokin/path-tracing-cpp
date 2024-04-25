#ifndef _CAMERA_H
#define _CAMERA_H

#include "math/Math.h"

class Camera {
public:
    constexpr Camera() noexcept = default;

    Camera(int viewportWidth, int viewportHeight, const Math::Vector3f &position = {0.f, 0.f, 0.f}, const Math::Vector3f &target = {0.f, 0.f, -1.f}, float verticalFovInDegrees = 20.f, const Math::Vector3f &up = {0.f, 1.f, 0.f}) noexcept;

    ~Camera() noexcept;

    void OnViewportResize(int viewportWidth, int viewportHeight) noexcept;

    void ComputeRayDirections() noexcept;

    constexpr Math::Vector3f* GetRayDirections() const noexcept {
        return m_RayDirections;
    }
 
    constexpr Math::Vector3f GetPosition() const noexcept {
        return m_Position;
    }

    constexpr Math::Vector3f& Position() noexcept {
        return m_Position;
    }

    constexpr Math::Vector3f& Target() noexcept {
        return m_Target;
    }

    constexpr Math::Vector3f& Up() noexcept {
        return m_Up;
    }

    constexpr float& VerticalFovInDegrees() noexcept {
        return m_VerticalFovInDegrees;
    }

private:
    Math::Vector3f m_Position;
    Math::Vector3f m_Target;
    Math::Vector3f m_Up;
    float m_VerticalFovInDegrees;
    int m_ViewportWidth, m_ViewportHeight;

    Math::Vector3f *m_RayDirections = nullptr;
};

#endif