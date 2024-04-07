#ifndef _CAMERA_H
#define _CAMERA_H

#include "../glm/include/glm/vec3.hpp"
#include "../glm/include/glm/trigonometric.hpp"

class Camera {
public:
    Camera(int viewportWidth, int viewportHeight, glm::vec3 position = {0.f, 0.f, 0.f}, glm::vec3 target = {0.f, 0.f, -1.f}, float verticalFovInRadians = glm::radians(20.f), glm::vec3 up = {0.f, 1.f, 0.f}) noexcept;

    ~Camera() noexcept;

    void OnViewportResize(int viewportWidth, int viewportHeight) noexcept;

    void ComputeRayDirections() noexcept;

    constexpr glm::vec3* GetRayDirections() const noexcept {
        return m_RayDirections;
    }
 
    constexpr glm::vec3 GetPosition() const noexcept {
        return m_Position;
    }

    constexpr void SetPosition(const glm::vec3 &position) noexcept {
        m_Position = position;
    }

    constexpr glm::vec3 GetTarget() const noexcept {
        return m_Target;
    }

    constexpr void SetTarget(const glm::vec3 &target) noexcept {
        m_Target = target;
    }

    constexpr glm::vec3 GetUp() const noexcept {
        return m_Up;
    }

    constexpr void SetUp(const glm::vec3 &up) noexcept {
        m_Up = up;
    }

    constexpr float GetVertivalFovInRadians() const noexcept {
        return m_VerticalFovInRadians;
    }

    constexpr void SetVerticalFovInRadians(float verticalFovInRadians) noexcept {
        m_VerticalFovInRadians = verticalFovInRadians;
    }

private:
    glm::vec3 m_Position;
    glm::vec3 m_Target;
    glm::vec3 m_Up;
    float m_VerticalFovInRadians;
    int m_ViewportWidth, m_ViewportHeight;

    glm::vec3 *m_RayDirections = nullptr;
};

#endif