#ifndef _RENDERER_H
#define _RENDERER_H

#include "../glm/include/glm/vec4.hpp"

#include "Image.h"
#include "Camera.h"
#include "Scene.h"
#include "HitPayload.h"
#include "Ray.h"
#include "Primitive.h"

#include <algorithm>
#include <functional>

class Renderer {
public:
    Renderer() = delete;
    
    Renderer(int width, int height) noexcept;

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

    void SaveImage(const char *filename) const noexcept;

    void OnResize(int width, int height) noexcept;

    constexpr int GetAvailableThreadCount() const noexcept {
        return m_AvailableThreads;
    }

    constexpr int GetUsedThreadCount() const noexcept {
        return m_UsedThreads;
    }

    constexpr void SetUsedThreadCount(int usedThreads) noexcept {
        m_UsedThreads = std::max(1, std::min(m_AvailableThreads, usedThreads));
        m_LinesPerThread = (m_Height + m_UsedThreads - 1) / m_UsedThreads;
    }

    inline void OnRayMiss(std::function<glm::vec3(const Ray&)> onRayMiss) noexcept {
        m_OnRayMiss = onRayMiss;
    }

    constexpr int GetMaxRayDepth() const noexcept {
        return m_MaxRayDepth;
    }

    inline void SetMaxRayDepth(int maxRayDepth) noexcept {
        m_MaxRayDepth = maxRayDepth;
    }

    constexpr float GetGamma() const noexcept {
        return m_Gamma;
    }

    inline void SetGamma(float gamma) noexcept {
        m_Gamma = gamma;
    }

private:
    glm::vec4 PixelProgram(int x, int y) const noexcept;

    HitPayload TraceRay(const Ray &ray) const noexcept;

    HitPayload ClosestHit(const Ray &ray, float t, Primitive primitive, int objectIndex) const noexcept;

    HitPayload Miss(const Ray &ray) const noexcept;

private:
    int m_Width, m_Height;
    Image *m_Image = nullptr;
    uint32_t *m_ImageData = nullptr;

    std::function<glm::vec3(const Ray&)> m_OnRayMiss = [](const Ray&){ return glm::vec3(0.f, 0.f, 0.f); };

    int m_AvailableThreads;
    int m_UsedThreads;
    int m_LinesPerThread;

    int m_MaxRayDepth = 5;

    const Camera *m_Camera = nullptr;
    const Scene *m_Scene = nullptr;

    bool m_Accumulate = false;
    glm::vec4 *m_AccumulationData = nullptr;
    int m_FrameIndex = 1;

    float m_Gamma = 2.f;
};

#endif