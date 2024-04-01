#ifndef _RENDERER_H
#define _RENDERER_H

#include "../glm/include/glm/vec4.hpp"

#include "Image.h"
#include "Camera.h"
#include "HitPayload.h"
#include "Ray.h"

class Renderer {
public:
    Renderer() = delete;
    
    inline Renderer(int width, int height, int maxRayDepth = 5) noexcept :
        m_Width(width), m_Height(height), m_MaxRayDepth(maxRayDepth), m_Camera(width, height) {}

    ~Renderer() noexcept;

    Image* Render() noexcept;

    void OnResize(int width, int height) noexcept;

private:
    glm::vec4 PixelProgram(int x, int y) const noexcept;

    HitPayload TraceRay(const Ray &ray) const noexcept;

    HitPayload ClosestHit(const Ray &ray, float t, int objectIndex) const noexcept;

    HitPayload Miss(const Ray &ray) const noexcept;

private:
    int m_Width, m_Height;
    Image *m_Image = nullptr;
    uint32_t *m_ImageData = nullptr;

    int m_MaxRayDepth;

    Camera m_Camera;
};

#endif