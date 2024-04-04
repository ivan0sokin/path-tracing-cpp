#ifndef _CAMERA_H
#define _CAMERA_H

#include "../glm/include/glm/vec3.hpp"
#include "../glm/include/glm/trigonometric.hpp"

class Camera {
public:
    Camera(int viewportWidth, int viewportHeight, glm::vec3 position = {0.f, 0.f, 0.f}, glm::vec3 target = {0.f, 0.f, -1.f}, glm::vec3 up = {0.f, 1.f, 0.f}, float verticalFovInRadians = glm::radians(20.f)) noexcept;

    ~Camera() noexcept;

    void OnViewportResize(int viewportWidth, int viewportHeight) noexcept;

    void ComputeRayDirections() noexcept;

    inline glm::vec3* GetRayDirections() const noexcept {
        return m_RayDirections;
    }
 
    inline glm::vec3 GetPosition() const noexcept {
        return m_Position;
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