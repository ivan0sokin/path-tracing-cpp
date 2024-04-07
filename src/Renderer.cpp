#include "Renderer.h"
#include "Utilities.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb-master/stb_image_write.h"

#include "../glm/include/glm/vec3.hpp"
#include "../glm/include/glm/geometric.hpp"

#include <vector>
#include <numeric>
#include <execution>
#include <thread>
#include <cstring>

Renderer::Renderer(int width, int height) noexcept :
    m_Width(width), m_Height(height) {
    m_ImageData = new uint32_t[m_Width * m_Height];
    m_Image = new Image(m_Width, m_Height, m_ImageData);
    m_AccumulationData = new glm::vec4[m_Width * m_Height];

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
        m_AccumulationData = new glm::vec4[m_Width * m_Height];
    }
}

void Renderer::Render(const Camera &camera, const Scene &scene) noexcept {
    m_Camera = &camera;
    m_Scene = &scene;

    if (!m_Accumulate) {
        m_FrameIndex = 1;
    }

    if (m_FrameIndex == 1) {
        memset(m_AccumulationData, 0, m_Width * m_Height * sizeof(glm::vec4));
    }

    float inverseFrameIndex = 1.f / m_FrameIndex;
    float inverseGamma = 1.f / m_Gamma;

    std::vector<std::thread> handles;
    handles.reserve(m_UsedThreads);

    for (int i = 0; i < m_Height; i += m_LinesPerThread) {
        handles.emplace_back([this, i, inverseFrameIndex, inverseGamma]() {
            int nextBlock = i + m_LinesPerThread;
            int limit = nextBlock < m_Height ? nextBlock : m_Height;
            for (int t = i; t < limit; ++t) {
                for (int j = 0; j < m_Width; ++j) {
                    m_AccumulationData[m_Width * t + j] += PixelProgram(t, j);

                    glm::vec4 color = m_AccumulationData[m_Width * t + j];
                    if (color.r != color.r) color.r = 0.f;
                    if (color.g != color.g) color.g = 0.f;
                    if (color.b != color.b) color.b = 0.f;

                    color *= inverseFrameIndex;
                    color = Utilities::CorrectGammaFast(color, inverseGamma);
                    color = glm::clamp(color, 0.f, 1.f);

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

glm::vec4 Renderer::PixelProgram(int i, int j) const noexcept {
    Ray ray;
    ray.origin = m_Camera->GetPosition();
    ray.direction = m_Camera->GetRayDirections()[m_Width * i + j];

    glm::vec3 light(0.f);
    glm::vec3 contribution(1.f);
    for (int i = 0; i < m_MaxRayDepth; ++i) {
        HitPayload payload = TraceRay(ray);

        if (payload.t < 0.f) {
            light += m_OnRayMiss(ray) * contribution;
            break;
        }

        int materialIndex = -1;
        switch (payload.primitive)
        {
        case Primitive::Sphere:
            materialIndex = m_Scene->spheres[payload.objectIndex].materialIndex;
            break;
        case Primitive::Triangle:
            materialIndex = m_Scene->triangles[payload.objectIndex].materialIndex;
            break;        
        default:
            break;
        }

        const Material &material = m_Scene->materials[materialIndex];
        
        light += material.GetEmission() * contribution;
        contribution *= material.albedo;

        ray.origin = payload.point + payload.normal * 0.00001f;
        if (Utilities::RandomFloatInZeroToOne() < material.reflectance) {
            ray.direction = glm::normalize(glm::reflect(ray.direction, payload.normal) + material.fuzziness * Utilities::RandomUnitVectorFast());
        } else {
            ray.direction = payload.normal + Utilities::RandomUnitVectorFast();

            if (Utilities::AlmostZero(ray.direction)) {
                ray.direction = payload.normal;
            } else {
                ray.direction = glm::normalize(ray.direction);
            }
        }
    }

    return {light, 1.f};
}

HitPayload Renderer::TraceRay(const Ray &ray) const noexcept {
    Primitive primitive = Primitive::None;
    int objectIndex = -1;
    float closestT = std::numeric_limits<float>::max();

    int objectCount = (int)m_Scene->spheres.size();
    for (int i = 0; i < objectCount; ++i) {
        const Sphere &sphere = m_Scene->spheres[i];

        glm::vec3 centerToOrigin = ray.origin - sphere.center;
        
        float a = glm::dot(ray.direction, ray.direction);
        float k = glm::dot(centerToOrigin, ray.direction);
        float c = glm::dot(centerToOrigin, centerToOrigin) - sphere.radiusSquared;
        float discriminant = k * k - a * c;
        
        if (discriminant < 0.0) {
            continue;
        }

        float t = (-k - glm::sqrt(discriminant)) / a;

        if (t > 0.f && t < closestT) {
            closestT = t;
            primitive = Primitive::Sphere;
            objectIndex = i;
        }
    }

    objectCount = (int)m_Scene->triangles.size();
    for (int i = 0; i < objectCount; ++i) {
        const Triangle &triangle = m_Scene->triangles[i];

        constexpr float epsilon = std::numeric_limits<float>::epsilon();

        glm::vec3 edge1 = triangle.edges[0];
        glm::vec3 edge2 = triangle.edges[1];
        glm::vec3 rayCrossEdge2 = glm::cross(ray.direction, edge2);
        float determinant = glm::dot(edge1, rayCrossEdge2);

        if (glm::abs(determinant) < epsilon) {
            continue;
        }

        float inverseDeterminant = 1.0 / determinant;
        glm::vec3 s = ray.origin - triangle.vertices[0];
        float u = inverseDeterminant * glm::dot(s, rayCrossEdge2);

        if (u < 0.f || u > 1.f) {
            continue;
        }

        glm::vec3 sCrossEdge1 = glm::cross(s, edge1);
        float v = inverseDeterminant * glm::dot(ray.direction, sCrossEdge1);

        if (v < 0.f || u + v > 1.f) {
            continue;
        }

        float t = inverseDeterminant * glm::dot(edge2, sCrossEdge1);

        if (t > 0.f && t < closestT) {
            closestT = t;
            primitive = Primitive::Triangle;
            objectIndex = i;
        }
    }

    if (objectIndex < 0) {
        return Miss(ray);
    }

    return ClosestHit(ray, closestT, primitive, objectIndex);
}

HitPayload Renderer::ClosestHit(const Ray &ray, float t, Primitive primitive, int objectIndex) const noexcept {
    HitPayload payload;
    payload.primitive = primitive;
    payload.objectIndex = objectIndex;
    payload.t = t;
    payload.point = ray.origin + ray.direction * t;

    switch (primitive) {
    case Primitive::Sphere: {
        const Sphere &sphere = m_Scene->spheres[objectIndex];
        payload.normal = (payload.point - sphere.center) * sphere.inverseRadius;
        break;       
    }
    case Primitive::Triangle: {
        const Triangle &triangle = m_Scene->triangles[objectIndex];
        payload.normal = triangle.normal;
        if (glm::dot(ray.direction, payload.normal) > 0) {
            payload.normal = -payload.normal;
        }
        payload.normal = glm::normalize(payload.normal);
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