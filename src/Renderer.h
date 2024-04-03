#ifndef _RENDERER_H
#define _RENDERER_H

#include "../glm/include/glm/vec4.hpp"

#include "Image.h"
#include "Camera.h"
#include "Scene.h"
#include "HitPayload.h"
#include "Ray.h"

class Renderer {
public:
    Renderer() = delete;
    
    Renderer(int width, int height, int maxRayDepth = 5) noexcept;

    ~Renderer() noexcept;

    void Render(const Camera &camera, const Scene &scene) noexcept;

    constexpr void Accumulate() noexcept {
        m_Accumulate = true;
    }

    constexpr void DontAccumulate() noexcept {
        m_Accumulate = false;
    }

    constexpr int GetFrameIndex() const noexcept {
        return m_FrameIndex;
    }

    Image* GetImage() const noexcept {
        return m_Image;
    }

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

    const Camera *m_Camera = nullptr;
    const Scene *m_Scene = nullptr;

    bool m_Accumulate = false;
    glm::vec4 *m_AccumulationData = nullptr;
    int m_FrameIndex = 1;
};

#endif