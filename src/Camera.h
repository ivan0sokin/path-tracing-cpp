#ifndef _CAMERA_H
#define _CAMERA_H

#include "math/Math.h"

class Camera {
public:
    Camera(int viewportWidth, int viewportHeight, const Math::Vector3f &position = {0.f, 0.f, 0.f}, const Math::Vector3f &target = {0.f, 0.f, -1.f}, float verticalFovInRadians = Math::ToRadians(20.f), const Math::Vector3f &up = {0.f, 1.f, 0.f}) noexcept;

    ~Camera() noexcept;

    void OnViewportResize(int viewportWidth, int viewportHeight) noexcept;

    void ComputeRayDirections() noexcept;

    constexpr Math::Vector3f* GetRayDirections() const noexcept {
        return m_RayDirections;
    }
 
    constexpr Math::Vector3f GetPosition() const noexcept {
        return m_Position;
    }

    constexpr void SetPosition(const Math::Vector3f &position) noexcept {
        m_Position = position;
    }

    constexpr Math::Vector3f GetTarget() const noexcept {
        return m_Target;
    }

    constexpr void SetTarget(const Math::Vector3f &target) noexcept {
        m_Target = target;
    }

    constexpr Math::Vector3f GetUp() const noexcept {
        return m_Up;
    }

    constexpr void SetUp(const Math::Vector3f &up) noexcept {
        m_Up = up;
    }

    constexpr float GetVertivalFovInRadians() const noexcept {
        return m_VerticalFovInRadians;
    }

    constexpr void SetVerticalFovInRadians(float verticalFovInRadians) noexcept {
        m_VerticalFovInRadians = verticalFovInRadians;
    }

private:
    Math::Vector3f m_Position;
    Math::Vector3f m_Target;
    Math::Vector3f m_Up;
    float m_VerticalFovInRadians;
    int m_ViewportWidth, m_ViewportHeight;

    Math::Vector3f *m_RayDirections = nullptr;
};

#endif