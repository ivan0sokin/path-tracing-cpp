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

void Renderer::Render(const Camera &camera, const Scene &scene) noexcept {
    m_Camera = &camera;
    m_Scene = &scene;

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
                    // auto clr = Li({m_Camera->GetPosition(), m_Camera->GetRayDirections()[m_Width * t + j]});
                    // m_AccumulationData[m_Width * t + j] += {clr.r, clr.g, clr.b, 1.f};

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

Math::Vector3f Renderer::Li(const Ray &ray, const Math::Vector3f &throughput, int depth) const noexcept {
    if (depth == m_MaxRayDepth) {
        return Math::Vector3f(0.f, 0.f, 0.f);
    }

    HitPayload payload = TraceRay(ray);
    if (payload.t < 0.f) {
        return m_OnRayMiss(ray);
    }

    const Material &material = m_Scene->materials[payload.materialIndex];
    Math::Vector3f emission = Math::Dot(ray.direction, payload.orientedNormal) <= 0.f ? material.GetEmission() : Math::Vector3f(0.f);
    
    Math::Vector3f indirect(1.f);
    BSDF bsdf(material);
    indirect *= Li({payload.point, bsdf.Sample(ray, payload, indirect)}, throughput, depth + 1);

    Math::Vector3f toLight(0.f);
    HitPayload lightHitPayload;
    float distance;
    float area;
    // {
        // sample light
        Math::Vector3f lightCenter(-278.f, 554.f, -278.f);
        float lightSize = 130.f;
        area = lightSize * lightSize;

        Math::Vector3f randomPointOnLight = lightCenter + Math::Vector3f{lightSize, 0.f, 0.f} * Utilities::RandomFloatInNegativeHalfToHalf()
                                                        + Math::Vector3f{0.f, 0.f, lightSize} * Utilities::RandomFloatInNegativeHalfToHalf();

        toLight = randomPointOnLight - payload.point;

        lightHitPayload = TraceRay({payload.point, Math::Normalize(toLight)});

    //     if (distance > lightHitPayload.t + 0.00001f) {
    //         lightPdf = 0.f;
    //     } else {
    //         float distanceSquared = distance * distance;
    //         float cosine = Math::Abs(Math::Dot(toLight, lightHitPayload.normal));

    //         // float G = Math::Max(0.f, Math::Dot(payload.normal, l_nee)) * Math::Max(0.f, -Math::Dot(l_nee, lightHitPayload.normal)) / rr_nee;

    //         lightPdf = distanceSquared / (cosine * lightSize * lightSize);
    //         // lightEmmision = m->payload.material.GetEmission();
    //         // lightPdf = 1.0 / (lightSize * lightSize * G);
    //     }
    // }

    // float v = 1.f;
    // float lightPdf = Math::Dot(toLight, toLight) / (Math::Abs(Math::Dot(Math::Normalize(toLight), lightHitPayload.normal)) * lightSize * lightSize);
    // if (lightHitPayload.t > Math::Length(toLight) + 0.001f) {
    //     v = 0.f;
    //     lightPdf = 0.f;
    // }

    // Math::Vector3f lightEmission = Math::Dot(toLight, m_Scene->triangles.back().orientedNormal) <= 0.f ? m_Scene->materials[m_Scene->triangles.back().materialIndex].GetEmission() : Math::Vector3f(0.f);

    // float P = Math::Dot(m_Scene->triangles.back().orientedNormal, -Math::Normalize(toLight)) / Math::Dot(toLight, toLight);
    // Math::Vector3f direct = v * material.albedo * Math::Constants::InversePi<float> * lightEmission * Math::Dot(payload.normal, Math::Normalize(toLight)) * P * area;

    // return emission + direct;
}

Math::Vector4f Renderer::PixelProgram(int i, int j) const noexcept {
    Ray ray;
    ray.origin = m_Camera->GetPosition();
    ray.direction = m_Camera->GetRayDirections()[m_Width * i + j];

    Math::Vector3f light(0.f), throughput(1.f);
    for (int i = 0; i < m_MaxRayDepth; ++i) {
        HitPayload payload = TraceRay(ray);
        
        if (payload.t < 0.f) {
            light += throughput * m_OnRayMiss(ray);
            break;
        }

        const Material &material = m_Scene->materials[payload.materialIndex];
        Math::Vector3f emission = Math::Dot(ray.direction, payload.orientedNormal) <= 0.f ? material.GetEmission() : Math::Vector3f(0.f);

        light += emission * throughput;

        BSDF bsdf(material);
        Math::Vector3f direction = bsdf.Sample(ray, payload, throughput);
        
        // Math::Vector3f lightCenter(-278.f, 554.f, -278.f);
        // float lightSize = 330.f;

        // Math::Vector3f randomPointOnLight = lightCenter + Math::Vector3f{lightSize, 0.f, 0.f} * Utilities::RandomFloatInNegativeHalfToHalf()
        //                                                 + Math::Vector3f{0.f, 0.f, lightSize} * Utilities::RandomFloatInNegativeHalfToHalf();

        

        ray.origin = payload.point;
        ray.direction = direction;

        // auto toLight = randomPointOnLight - payload.point;
        // auto lightCosine = fabs(Math::Normalize(toLight).y);

        // auto lightHitPayload = TraceRay({payload.point, Math::Normalize(toLight)});

        

        // float lightArea = lightSize * lightSize;
        // float lightPdf = Math::Dot(toLight, toLight) / (lightCosine * lightArea);

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

HitPayload Renderer::TraceRay(const Ray &ray) const noexcept {
    int objectIndex = -1;
    int polygonIndex = -1;
    Math::Vector3f worldNormal;
    float closestT = Math::Constants::Max<float>;

    int objectCount = (int)m_Scene->objects.size();
    for (int i = 0; i < objectCount; ++i) {
        auto [t, normal] = m_Scene->objects[i]->TryHit(ray);

        if (t > 0.001f && t < closestT) {
            closestT = t;
            objectIndex = i;
            worldNormal = normal;
        }
    }

    if (objectIndex < 0) {
        return Miss(ray);
    }

    return ClosestHit(ray, closestT, objectIndex, polygonIndex, worldNormal);
}

HitPayload Renderer::ClosestHit(const Ray &ray, float t, int objectIndex, int polygonIndex, const Math::Vector3f &worldNormal) const noexcept {
    // HitPayload payload;
    // payload.t = t;
    // payload.point = ray.origin + ray.direction * t;

    // switch (primitive) {
    // case Primitive::Sphere: {
    //     const Shapes::Sphere &sphere = m_Scene->spheres[objectIndex];
    //     payload.normal = (payload.point - sphere.center) * sphere.inverseRadius;
    //     payload.orientedNormal = payload.normal;
    //     payload.materialIndex = sphere.materialIndex;
    //     break;       
    // }
    // case Primitive::Triangle: {
    //     const Shapes::Triangle &triangle = m_Scene->triangles[objectIndex];

    //     payload.normal = triangle.orientedNormal;
    //     if (Math::Dot(ray.direction, payload.normal) > Math::Constants::Epsilon<float>) {
    //         payload.normal = -payload.normal;
    //     }
    //     payload.normal = Math::Normalize(payload.normal);

    //     payload.orientedNormal = triangle.orientedNormal;
    //     payload.materialIndex = triangle.materialIndex;
    //     break;
    // }
    // case Primitive::Box: {
    //     const Shapes::Triangle &triangle = m_Scene->boxes[objectIndex].triangles[polygonIndex];

    //     payload.normal = triangle.orientedNormal;
    //     if (Math::Dot(ray.direction, payload.normal) > Math::Constants::Epsilon<float>) {
    //         payload.normal = -payload.normal;
    //     }
    //     payload.normal = Math::Normalize(payload.normal);

    //     payload.orientedNormal = triangle.orientedNormal;
    //     payload.materialIndex = triangle.materialIndex;
    //     break;
    // }
    // default:
    //     break;
    // }
    
    // return payload;

    HitPayload payload;
    payload.t = t;
    payload.point = ray.origin + ray.direction * t;

    HittableObject *object = m_Scene->objects[objectIndex];
    payload.orientedNormal = worldNormal;
    payload.normal = Math::Dot(ray.direction, payload.normal) > Math::Constants::Epsilon<float> ? -payload.orientedNormal : payload.orientedNormal;
    payload.materialIndex = object->GetMaterialIndex();

    return payload;
}

HitPayload Renderer::Miss(const Ray &ray) const noexcept {
    HitPayload payload;
    payload.t = -1.f;
    return payload;
}