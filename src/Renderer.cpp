#include "Renderer.h"
#include "Utilities.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb-master/stb_image_write.h"

#include "../glm/include/glm/vec3.hpp"
#include "../glm/include/glm/geometric.hpp"

#include <algorithm>
#include <cstring>

Renderer::Renderer(int width, int height) noexcept :
    m_Width(width), m_Height(height) {
    m_ImageData = new uint32_t[m_Width * m_Height];
    m_Image = new Image(m_Width, m_Height, m_ImageData);
    m_AccumulationData = new glm::vec4[m_Width * m_Height];
}

Renderer::~Renderer() noexcept {
    if (m_Image != nullptr) {
        delete m_Image;
    }
    if (m_ImageData != nullptr) {
        delete m_ImageData;
    }
    if (m_AccumulationData != nullptr) {
        delete m_AccumulationData;
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

    for (int i = 0; i < m_Height; ++i) {
        for (int j = 0; j < m_Width; ++j) {
            m_AccumulationData[m_Width * i + j] += PixelProgram(i, j);

            glm::vec4 color = m_AccumulationData[m_Width * i + j] * inverseFrameIndex;
            color = Utilities::CorrectGammaFast(color, inverseGamma);
            color = glm::clamp(color, 0.f, 1.f);
            m_ImageData[m_Width * i + j] = Utilities::ConvertColorToRGBA(color);
        }
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
            glm::vec3 skyColor(0.6f, 0.7f, 0.9f);
            // light += skyColor * contribution;
            break;
        }

        const Sphere &sphere = m_Scene->spheres[payload.objectIndex];
        const Material &material = m_Scene->materials[sphere.materialIndex];

        light += material.GetEmission() * contribution;
        contribution *= material.albedo;

        ray.origin = payload.point + payload.normal * 0.0001f;
        ray.direction = glm::normalize(payload.normal + Utilities::RandomUnitVector());
    }

    return {light, 1.f};
}

HitPayload Renderer::TraceRay(const Ray &ray) const noexcept {
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
            objectIndex = i;
        }
    }

    if (objectIndex < 0) {
        return Miss(ray);
    }

    return ClosestHit(ray, closestT, objectIndex);
}

HitPayload Renderer::ClosestHit(const Ray &ray, float t, int objectIndex) const noexcept {
    HitPayload payload;
    payload.objectIndex = objectIndex;
    payload.t = t;

    const Sphere &sphere = m_Scene->spheres[objectIndex];

    payload.point = ray.origin + ray.direction * t;
    payload.normal = (payload.point - sphere.center) * sphere.inverseRadius;

    return payload;
}

HitPayload Renderer::Miss(const Ray &ray) const noexcept {
    HitPayload payload;
    payload.t = -1.f;
    return payload;
}