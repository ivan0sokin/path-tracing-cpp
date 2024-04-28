#include "Renderer.h"
#include "Utilities.hpp"
#include "BSDF.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb-master/stb_image_write.h"

#include <vector>
#include <numeric>
#include <execution>
#include <thread>
#include <cstring>

Renderer::Renderer(int width, int height) noexcept :
    m_Width(width), m_Height(height) {
    m_ImageData = new uint32_t[m_Width * m_Height];
    m_Image = new Image(m_Width, m_Height, m_ImageData);
    m_AccumulationData = new Math::Vector4f[m_Width * m_Height];

    m_AvailableThreads = std::thread::hardware_concurrency();
    m_UsedThreads = 1;
    m_LinesPerThread = (m_Height + m_UsedThreads - 1) / m_UsedThreads;
}

Renderer::~Renderer() noexcept {
    if (m_Image != nullptr) {
        delete m_Image;
    }
    if (m_ImageData != nullptr) {
        delete[] m_ImageData;
    }
    if (m_AccumulationData != nullptr) {
        delete[] m_AccumulationData;
    }
}

void Renderer::SaveImage(const char *filename) const noexcept {
    stbi_write_png(filename, m_Width, m_Height, 4, m_ImageData, m_Width * 4);
}

void Renderer::OnResize(int width, int height) noexcept {
    if (m_Width == width && m_Height == height) {
        return;
    }
    
    m_Width = width;
    m_Height = height;

    if (m_ImageData != nullptr) {
        delete m_ImageData;
        m_ImageData = new uint32_t[m_Width * m_Height];
    }
    if (m_Image != nullptr) {
        delete m_Image;
        m_Image = new Image(m_Width, m_Height, m_ImageData);
    }
    if (m_AccumulationData != nullptr) {
        delete m_AccumulationData;
        m_AccumulationData = new Math::Vector4f[m_Width * m_Height];
    }
}

void Renderer::Render(const Camera &camera, std::span<const HittableObjectPtr> objects, std::span<const Material> materials) noexcept {
    m_Camera = &camera;
    m_Objects = objects;
    m_Materials = materials;

    if (!m_Accumulate) {
        m_FrameIndex = 1;
    }

    if (m_FrameIndex == 1) {
        memset(m_AccumulationData, 0, m_Width * m_Height * sizeof(Math::Vector4f));
    }

    float inverseFrameIndex = 1.f / m_FrameIndex;
    float inverseGamma = 1.f / m_Gamma;

    std::vector<std::thread> handles;
    handles.reserve(m_UsedThreads);

    for (int i = 0; i < m_Height; i += m_LinesPerThread) {
        handles.emplace_back([this, i, inverseFrameIndex, inverseGamma]() {
            int nextBlock = i + m_LinesPerThread;
            int limit = Math::Min(nextBlock, m_Height);
            for (int t = i; t < limit; ++t) {
                for (int j = 0; j < m_Width; ++j) {
                    m_AccumulationData[m_Width * t + j] += PixelProgram(t, j);

                    Math::Vector4f color = m_AccumulationData[m_Width * t + j];

                    color *= inverseFrameIndex;
                    color = Utilities::CorrectGamma(color, inverseGamma);
                    color = Math::Clamp(color, 0.f, 1.f);

                    m_ImageData[m_Width * t + j] = Utilities::ConvertColorToRGBA(color);
                }
            }
        });
    }

    for (auto &handle : handles) {
        handle.join();
    }

    m_Image->UpdateData(m_ImageData);

    if (m_Accumulate) {
        ++m_FrameIndex;
    }
}

void Renderer::Render(const Camera &camera, const AccelerationStructure &accelerationStructure, std::span<const Material> materials) noexcept {
    m_Camera = &camera;
    m_AccelerationStructure = &accelerationStructure;
    m_Materials = materials;

    if (!m_Accumulate) {
        m_FrameIndex = 1;
    }

    if (m_FrameIndex == 1) {
        memset(m_AccumulationData, 0, m_Width * m_Height * sizeof(Math::Vector4f));
    }

    float inverseFrameIndex = 1.f / m_FrameIndex;
    float inverseGamma = 1.f / m_Gamma;

    std::vector<std::thread> handles;
    handles.reserve(m_UsedThreads);

    for (int i = 0; i < m_Height; i += m_LinesPerThread) {
        handles.emplace_back([this, i, inverseFrameIndex, inverseGamma]() {
            int nextBlock = i + m_LinesPerThread;
            int limit = Math::Min(nextBlock, m_Height);
            for (int t = i; t < limit; ++t) {
                for (int j = 0; j < m_Width; ++j) {
                    m_AccumulationData[m_Width * t + j] += AcceleratedPixelProgram(t, j);

                    Math::Vector4f color = m_AccumulationData[m_Width * t + j];

                    color *= inverseFrameIndex;
                    color = Utilities::CorrectGamma(color, inverseGamma);
                    color = Math::Clamp(color, 0.f, 1.f);

                    m_ImageData[m_Width * t + j] = Utilities::ConvertColorToRGBA(color);
                }
            }
        });
    }

    for (auto &handle : handles) {
        handle.join();
    }

    m_Image->UpdateData(m_ImageData);

    if (m_Accumulate) {
        ++m_FrameIndex;
    }
}

Math::Vector4f Renderer::PixelProgram(int i, int j) const noexcept {
    Ray ray;
    ray.origin = m_Camera->GetPosition();
    ray.direction = m_Camera->GetRayDirections()[m_Width * i + j];

    Math::Vector3f light(0.f), throughput(1.f);
    for (int i = 0; i < m_RayDepth; ++i) {
        HitPayload payload = TraceRay(ray);
        
        if (payload.t < 0.f) {
            light += throughput * m_OnRayMiss(ray);
            break;
        }

        const Material &material = m_Materials[payload.materialIndex];
        Math::Vector3f emission = material.GetEmission();

        light += emission * throughput;

        BSDF bsdf(material);
        auto direction = bsdf.Sample(ray, payload, throughput);
        
        // Math::Vector3f lightCenter(-278.f, 554.f, -278.f);
        // float lightSize = 330.f;

        // Math::Vector3f randomPointOnLight = lightCenter + Math::Vector3f{lightSize, 0.f, 0.f} * Utilities::RandomFloatInNegativeHalfToHalf()
        //                                                 + Math::Vector3f{0.f, 0.f, lightSize} * Utilities::RandomFloatInNegativeHalfToHalf();
        
        ray.origin += ray.direction * payload.t;
        ray.direction = direction;

        // auto toLight = randomPointOnLight - ray.origin;
        // auto lightCosine = fabs(Math::Normalize(toLight).y);

        // auto lightHitPayload = TraceRay({ray.origin, Math::Normalize(toLight)});

        // float lightArea = lightSize * lightSize;
        // float lightPdf = Math::Dot(toLight, toLight) / (lightCosine * lightArea);

        // if (pdf > Math::Constants::Epsilon<float>) { // todo ! back
            // throughput /= pdf;
        // }

        // if (lightCosine > 0.0001f) {
        //     light += throughput * 1.f;
        //     throughput /= lightPdf;
        // }

        // ray.direction = direction;
        // if (pdf > Math::Constants::Epsilon<float>) {
        //     if (lightCosine < 0.0001f || lightHitPayload.t + 0.001f < Math::Length(toLight)) {
        //         throughput /= pdf;
        //     } else {
        //         float f = lightPdf;
        //         float g = pdf;
        //         float totalPdf = (f * f) / (f * f + g * g);

        //         throughput /= (pdf * 0.5f + totalPdf * 0.5f);
        //         if (Utilities::RandomFloatInZeroToOne() < 0.5f) {
        //             ray.direction = toLight;
        //         }
        //     }
        // }

        // if ((lightCosine < 0.0001f || lightHitPayload.t + 0.001f < Math::Length(toLight)) && pdf > Math::Constants::Epsilon<float>) {
        //     throughput /= pdf;
        //     ray.direction = direction;
        // } else if (!(lightCosine < 0.0001f || lightHitPayload.t + 0.001f < Math::Length(toLight)) && pdf > Math::Constants::Epsilon<float>) {
        //     throughput /= (0.5f * lightPdf + 0.5f * pdf);

        //     if (Utilities::RandomFloatInZeroToOne() < 0.5f) {
        //         ray.direction = toLight;
        //     } else {
        //         ray.direction = direction;
        //     }
        // } else if (!(lightCosine < 0.0001f || lightHitPayload.t + 0.001f < Math::Length(toLight))) {
        //     // throughput /= lightPdf;
        //     ray.direction = direction;
        //     // ray.direction = toLight;
        // } else {
        //     ray.direction = direction;
        // }

        // if (Utilities::RandomFloatInZeroToOne() < 0.5f) {
        //     ray.direction = toLight;
        //     if (lightCosine < 0.0001f || lightHitPayload.t + 0.001f < Math::Length(toLight)) {
        //         // light += throughput * m_OnRayMiss(ray);
        //         break;
        //     }
        // throughput /= lightPdf;

        // } else {
        //     ray.direction = direction;
        // }
        
    }
 
    return {light.r, light.g, light.b, 1.f};
}

Math::Vector4f Renderer::AcceleratedPixelProgram(int i, int j) const noexcept {
    Ray ray;
    ray.origin = m_Camera->GetPosition();
    ray.direction = m_Camera->GetRayDirections()[m_Width * i + j];

    Math::Vector3f light(0.f), throughput(1.f);
    for (int i = 0; i < m_RayDepth; ++i) {
        HitPayload payload = AcceleratedTraceRay(ray);
        
        if (payload.t < 0.f) {
            light += throughput * m_OnRayMiss(ray);
            break;
        }

        const Material &material = m_Materials[payload.materialIndex];
        Math::Vector3f emission = material.GetEmission();

        light += emission * throughput;

        BSDF bsdf(material);
        auto direction = bsdf.Sample(ray, payload, throughput);

        ray.origin += ray.direction * payload.t;
        ray.direction = direction;
    }
 
    return {light.r, light.g, light.b, 1.f};
}

HitPayload Renderer::TraceRay(const Ray &ray) const noexcept {
    HitPayload payload;
    payload.t = Math::Constants::Infinity<float>;
    payload.normal = Math::Vector3f(0.f);
    payload.materialIndex = -1;

    int objectCount = (int)m_Objects.size();
    for (int i = 0; i < objectCount; ++i) {
        m_Objects[i]->Hit(ray, 0.01f, Math::Min(payload.t, Math::Constants::Infinity<float>), payload);
    }

    if (payload.materialIndex == -1) {
        return Miss(ray);
    }

    payload.normal = Math::Dot(ray.direction, payload.normal) > Math::Constants::Epsilon<float> ? -payload.normal : payload.normal;

    return payload;
}

HitPayload Renderer::AcceleratedTraceRay(const Ray &ray) const noexcept {
    HitPayload payload;
    payload.t = Math::Constants::Infinity<float>;
    payload.normal = Math::Vector3f(0.f);
    payload.materialIndex = -1;

    m_AccelerationStructure->Hit(ray, 0.01f, Math::Constants::Infinity<float>, payload);

    if (payload.materialIndex == -1) {
        return Miss(ray);
    }

    payload.normal = Math::Dot(ray.direction, payload.normal) > Math::Constants::Epsilon<float> ? -payload.normal : payload.normal;

    return payload;
}

HitPayload Renderer::Miss(const Ray &ray) const noexcept {
    HitPayload payload;
    payload.t = -1.f;
    return payload;
}