#ifndef _CAMERA_H
#define _CAMERA_H

#include "math/Math.h"

#include <vector>
#include <span>

class Camera {
public:
    inline Camera() noexcept = default;

    Camera(int viewportWidth, int viewportHeight, const Math::Vector3f &position = {0.f, 0.f, 0.f}, const Math::Vector3f &target = {0.f, 0.f, -1.f}, float verticalFovInDegrees = 20.f, const Math::Vector3f &up = {0.f, 1.f, 0.f}) noexcept;

    ~Camera() noexcept = default;

    void OnViewportResize(int viewportWidth, int viewportHeight) noexcept;

    void ComputeRayDirections() noexcept;

    constexpr std::span<const Math::Vector3f> GetRayDirections() const noexcept {
        return m_RayDirections;
    }
 
    constexpr Math::Vector3f GetPosition() const noexcept {
        return m_Position;
    }

    constexpr Math::Vector3f& Position() noexcept {
        return m_Position;
    }

    constexpr Math::Vector3f GetTarget() const noexcept {
        return m_Target;
    }

    constexpr Math::Vector3f& Target() noexcept {
        return m_Target;
    }

    constexpr Math::Vector3f GetUp() const noexcept {
        return m_Up;
    }

    constexpr Math::Vector3f& Up() noexcept {
        return m_Up;
    }

    constexpr float GetVerticalFovInDegrees() const noexcept {
        return m_VerticalFovInDegrees;
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

    std::vector<Math::Vector3f> m_RayDirections;
};

#endif