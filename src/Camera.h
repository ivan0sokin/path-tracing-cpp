#ifndef _CAMERA_H
#define _CAMERA_H

#include "math/Math.h"

#include <vector>
#include <span>

//! Observerer class. Perspective projection onto screen
class Camera {
public:
    inline Camera() noexcept = default;

    //! Constructs Camera with given parameters
    Camera(int viewportWidth, int viewportHeight, const Math::Vector3f &position = {0.f, 0.f, 0.f}, const Math::Vector3f &target = {0.f, 0.f, -1.f}, float verticalFovInDegrees = 20.f, const Math::Vector3f &up = {0.f, 1.f, 0.f}) noexcept;

    ~Camera() noexcept = default;

    //! Reconstructs internal state based on new viewport size
    void OnViewportResize(int viewportWidth, int viewportHeight) noexcept;

    //! Computes ray directions and saves them
    void ComputeRayDirections() noexcept;

    //! Return span to array of ray directions
    constexpr std::span<const Math::Vector3f> GetRayDirections() const noexcept {
        return m_RayDirections;
    }
 
    //! Returns position of Camera
    constexpr Math::Vector3f GetPosition() const noexcept {
        return m_Position;
    }

    //! Returns reference to position. GUI convenience
    constexpr Math::Vector3f& Position() noexcept {
        return m_Position;
    }

    //! Returns target of Camera. GUI convenience
    constexpr Math::Vector3f GetTarget() const noexcept {
        return m_Target;
    }

    //! Returns reference to target. GUI convenience
    constexpr Math::Vector3f& Target() noexcept {
        return m_Target;
    }

    //! Returns up vector of Camera
    constexpr Math::Vector3f GetUp() const noexcept {
        return m_Up;
    }

    //! Returns reference to up vector. GUI convenience
    constexpr Math::Vector3f& Up() noexcept {
        return m_Up;
    }

    //! Returns vertical fov of Camera in degrees
    constexpr float GetVerticalFovInDegrees() const noexcept {
        return m_VerticalFovInDegrees;
    }

    //! Returns reference to vertical fov in degrees. GUI convenience
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