#ifndef _RENDERER_H
#define _RENDERER_H

#include "Image.h"
#include "Camera.h"
#include "Scene.h"
#include "HitPayload.h"
#include "Ray.h"
#include "math/Math.h"
#include "AccelerationStructure.h"
#include "Material.h"

#include <functional>
#include <span>

class Renderer {
public:
    Renderer() = delete;
    
    Renderer(int width, int height) noexcept;

    ~Renderer() noexcept;

    using typ_t = HittableObject*;

    void Render(const Camera &camera, std::span<const HittableObjectPtr> objects, std::span<const Material> materials) noexcept;

    void Render(const Camera &camera, const AccelerationStructure &accelerationStructure, std::span<const Material> materials) noexcept;

    constexpr bool& Accumulate() noexcept {
        return m_Accumulate;
    }

    constexpr bool& Accelerate() noexcept {
        return m_Accelerate;
    }

    constexpr int GetFrameIndex() const noexcept {
        return m_FrameIndex;
    }

    constexpr Image* GetImage() const noexcept {
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
        m_UsedThreads = Math::Clamp(usedThreads, 1, m_AvailableThreads);
        m_LinesPerThread = (m_Height + m_UsedThreads - 1) / m_UsedThreads;
    }

    constexpr int& UsedThreadCount() noexcept {
        return m_UsedThreads;
    }

    inline void OnRayMiss(std::function<Math::Vector3f(const Ray&)> onRayMiss) noexcept {
        m_OnRayMiss = onRayMiss;
    }

    constexpr int& RayDepth() noexcept {
        return m_RayDepth;
    }

    constexpr float& Gamma() noexcept {
        return m_Gamma;
    }

private:
    Math::Vector4f PixelProgram(int u, int j) const noexcept;

    Math::Vector4f AcceleratedPixelProgram(int i, int j) const noexcept;

    HitPayload TraceRay(const Ray &ray) const noexcept;

    HitPayload AcceleratedTraceRay(const Ray &ray) const noexcept;

    HitPayload Miss(const Ray &ray) const noexcept;

private:
    int m_Width, m_Height;
    Image *m_Image = nullptr;
    uint32_t *m_ImageData = nullptr;

    std::function<Math::Vector3f(const Ray&)> m_OnRayMiss = [](const Ray&){ return Math::Vector3f(0.f, 0.f, 0.f); };

    int m_AvailableThreads;
    int m_UsedThreads;
    int m_LinesPerThread;

    int m_RayDepth = 5;

    const Camera *m_Camera = nullptr;
    std::span<const HittableObjectPtr> m_Objects;
    const AccelerationStructure *m_AccelerationStructure = nullptr;
    std::span<const Material> m_Materials;

    bool m_Accumulate = false;
    Math::Vector4f *m_AccumulationData = nullptr;
    int m_FrameIndex = 1;

    bool m_Accelerate = false;

    float m_Gamma = 2.f;
};

#endif