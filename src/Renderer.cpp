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
    
    return emission + material.albedo * Li({payload.point, Utilities::RandomUnitVector()}, throughput, depth + 1);
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

        ray.origin = payload.point;
        ray.direction = direction;
    }

    return {light.r, light.g, light.b, 1.f};
}

HitPayload Renderer::TraceRay(const Ray &ray) const noexcept {
    Primitive primitive = Primitive::None;
    int objectIndex = -1;
    int polygonIndex = -1;
    float closestT = std::numeric_limits<float>::max();

    int objectCount = (int)m_Scene->spheres.size();
    for (int i = 0; i < objectCount; ++i) {
        const Shapes::Sphere &sphere = m_Scene->spheres[i];

        Math::Vector3f centerToOrigin = ray.origin - sphere.center;
        
        float a = Math::Dot(ray.direction, ray.direction);
        float k = Math::Dot(centerToOrigin, ray.direction);
        float c = Math::Dot(centerToOrigin, centerToOrigin) - sphere.radiusSquared;
        float discriminant = k * k - a * c;
        
        if (discriminant < 0.0) {
            continue;
        }

        float t = (-k - Math::Sqrt(discriminant)) / a;

        if (t > 0.001f && t < closestT) {
            closestT = t;
            primitive = Primitive::Sphere;
            objectIndex = i;
        }
    }

    objectCount = (int)m_Scene->triangles.size();
    for (int i = 0; i < objectCount; ++i) {
        const Shapes::Triangle &triangle = m_Scene->triangles[i];

        constexpr float epsilon = Math::Constants::Epsilon<float>;

        Math::Vector3f edge1 = triangle.edges[0];
        Math::Vector3f edge2 = triangle.edges[1];
        Math::Vector3f rayCrossEdge2 = Math::Cross(ray.direction, edge2);
        float determinant = Math::Dot(edge1, rayCrossEdge2);

        if (Math::Abs(determinant) < epsilon) {
            continue;
        }

        float inverseDeterminant = 1.0 / determinant;
        Math::Vector3f s = ray.origin - triangle.vertices[0];
        float u = inverseDeterminant * Math::Dot(s, rayCrossEdge2);

        if (u < 0.f || u > 1.f) {
            continue;
        }

        Math::Vector3f sCrossEdge1 = Math::Cross(s, edge1);
        float v = inverseDeterminant * Math::Dot(ray.direction, sCrossEdge1);

        if (v < 0.f || u + v > 1.f) {
            continue;
        }

        float t = inverseDeterminant * Math::Dot(edge2, sCrossEdge1);

        if (t > 0.001f && t < closestT) {
            closestT = t;
            primitive = Primitive::Triangle;
            objectIndex = i;
        }
    }

    objectCount = (int)m_Scene->boxes.size();
    for (int i = 0; i < objectCount; ++i) {
        const Shapes::Box &box = m_Scene->boxes[i];
        for (int j = 0; j < 12; ++j) {
            const Shapes::Triangle &triangle = box.triangles[j];

            constexpr float epsilon = Math::Constants::Epsilon<float>;

            Math::Vector3f edge1 = triangle.edges[0];
            Math::Vector3f edge2 = triangle.edges[1];
            Math::Vector3f rayCrossEdge2 = Math::Cross(ray.direction, edge2);
            float determinant = Math::Dot(edge1, rayCrossEdge2);

            if (Math::Abs(determinant) < epsilon) {
                continue;
            }

            float inverseDeterminant = 1.0 / determinant;
            Math::Vector3f s = ray.origin - triangle.vertices[0];
            float u = inverseDeterminant * Math::Dot(s, rayCrossEdge2);

            if (u < 0.f || u > 1.f) {
                continue;
            }

            Math::Vector3f sCrossEdge1 = Math::Cross(s, edge1);
            float v = inverseDeterminant * Math::Dot(ray.direction, sCrossEdge1);

            if (v < 0.f || u + v > 1.f) {
                continue;
            }

            float t = inverseDeterminant * Math::Dot(edge2, sCrossEdge1);

            if (t > 0.001f && t < closestT) {
                closestT = t;
                primitive = Primitive::Box;
                objectIndex = i;
                polygonIndex = j;
            }
        }
    }

    if (objectIndex < 0) {
        return Miss(ray);
    }

    return ClosestHit(ray, closestT, primitive, objectIndex, polygonIndex);
}

HitPayload Renderer::ClosestHit(const Ray &ray, float t, Primitive primitive, int objectIndex, int polygonIndex) const noexcept {
    HitPayload payload;
    payload.t = t;
    payload.point = ray.origin + ray.direction * t;

    switch (primitive) {
    case Primitive::Sphere: {
        const Shapes::Sphere &sphere = m_Scene->spheres[objectIndex];
        payload.normal = (payload.point - sphere.center) * sphere.inverseRadius;
        payload.orientedNormal = payload.normal;
        payload.materialIndex = sphere.materialIndex;
        break;       
    }
    case Primitive::Triangle: {
        const Shapes::Triangle &triangle = m_Scene->triangles[objectIndex];

        payload.normal = triangle.orientedNormal;
        if (Math::Dot(ray.direction, payload.normal) > Math::Constants::Epsilon<float>) {
            payload.normal = -payload.normal;
        }
        payload.normal = Math::Normalize(payload.normal);

        payload.orientedNormal = triangle.orientedNormal;
        payload.materialIndex = triangle.materialIndex;
        break;
    }
    case Primitive::Box: {
        const Shapes::Triangle &triangle = m_Scene->boxes[objectIndex].triangles[polygonIndex];

        payload.normal = triangle.orientedNormal;
        if (Math::Dot(ray.direction, payload.normal) > Math::Constants::Epsilon<float>) {
            payload.normal = -payload.normal;
        }
        payload.normal = Math::Normalize(payload.normal);

        payload.orientedNormal = triangle.orientedNormal;
        payload.materialIndex = triangle.materialIndex;
        break;
    }
    default:
        break;
    }
    
    return payload;
}

HitPayload Renderer::Miss(const Ray &ray) const noexcept {
    HitPayload payload;
    payload.t = -1.f;
    return payload;
}