#ifndef _RENDERER_H
#define _RENDERER_H

#include "image/Image.h"
#include "Camera.h"
#include "Scene.h"
#include "HitPayload.h"
#include "Ray.h"
#include "math/Math.h"
#include "Material.h"
#include "Light.h"
#include "acceleration/TLAS.h"

#include <functional>
#include <span>

//! Class that renders Scene to Image
class Renderer {
public:
    Renderer() = delete;
    
    //! Creates renderer with given width and height
    Renderer(int width, int height) noexcept;

    //! Deallocates image data
    ~Renderer() noexcept;

    //! Renders without object acceleration (but with model accelerator for speed purpose)
    void Render(const Camera &camera, std::span<IHittable* const> objects, std::span<const Light> lightSources, std::span<const Material> materials) noexcept;

    //! Renders with object acceleration
    void Render(const Camera &camera, const TLAS *accelerationStructure, std::span<const Light> lightSources, std::span<const Material> materials) noexcept;

    //! Returns reference to accumulation flag. GUI convinience
    constexpr bool& Accumulate() noexcept {
        return m_Accumulate;
    }

    //! Returns reference to acceleration flag. GUI convinience
    constexpr bool& Accelerate() noexcept {
        return m_Accelerate;
    }

    //! Returns current frame index
    constexpr int GetFrameIndex() const noexcept {
        return m_FrameIndex;
    }

    //! Returns pointer to rendered Image
    constexpr Image* GetImage() const noexcept {
        return m_Image;
    }

    //! Resizes Image
    void OnResize(int width, int height) noexcept;

    //! Returns not used number of threads
    constexpr int GetAvailableThreadCount() const noexcept {
        return m_AvailableThreads;
    }

    //! Returns number of threads used in rendering
    constexpr int GetUsedThreadCount() const noexcept {
        return m_UsedThreads;
    }

    //! Sets used threads
    constexpr void SetUsedThreadCount(int usedThreads) noexcept {
        m_UsedThreads = Math::Clamp(usedThreads, 1, m_AvailableThreads);
        m_LinesPerThread = (m_Height + m_UsedThreads - 1) / m_UsedThreads;
    }

    //! Returns reference to number of threads used in rendering. GUI convinience
    constexpr int& UsedThreadCount() noexcept {
        return m_UsedThreads;
    }

    //! Sets rule on which missing ray is lightened
    inline void OnRayMiss(std::function<Math::Vector3f(const Ray&)> onRayMiss) noexcept {
        m_OnRayMiss = onRayMiss;
    }

    //! Returns reference to ray depth. GUI convinience
    constexpr int& RayDepth() noexcept {
        return m_RayDepth;
    }
    
    //! Returns reference to gamma. GUI convinience
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
    Image *m_Image;

    int m_AvailableThreads;
    int m_UsedThreads;
    int m_LinesPerThread;

    int m_RayDepth = 5;

    std::function<Math::Vector3f(const Ray&)> m_OnRayMiss = [](const Ray&){ return Math::Vector3f(0.f, 0.f, 0.f); };

    const Camera *m_Camera = nullptr;
    std::span<IHittable* const> m_Objects;
    std::span<const Light> m_LightSources;
    const TLAS *m_AccelerationStructure = nullptr;
    std::span<const Material> m_Materials;

    bool m_Accumulate = false;
    Math::Vector4f *m_AccumulationData = nullptr;
    int m_FrameIndex = 1;

    bool m_Accelerate = false;

    float m_Gamma = 2.f;
};

#endif