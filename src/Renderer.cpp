#include "Renderer.h"
#include "Utilities.hpp"

#include "../glm/include/glm/vec3.hpp"

Renderer::~Renderer() noexcept {
    if (m_Image != nullptr) {
        delete m_Image;
    }
    if (m_ImageData != nullptr) {
        delete m_ImageData;
    }
}

void Renderer::OnResize(int width, int height) noexcept {
    if (m_Width == width && m_Height == height) {
        return;
    }
    
    m_Width = width;
    m_Height = height;

    delete m_ImageData;
    delete m_Image;

    m_ImageData = nullptr;
    m_Image = nullptr;
}

Image* Renderer::Render() noexcept {
    if (m_Image != nullptr) {
        return m_Image;
    }

    m_ImageData = new uint32_t[m_Width * m_Height];

    for (int i = 0; i < m_Height; ++i) {
        for (int j = 0; j < m_Width; ++j) {
            m_ImageData[m_Width * i + j] = Utilities::ConvertColorToRGBA(PixelProgram(i, j));
        }
    }

    m_Image = new Image(m_Width, m_Height, m_ImageData);
    return m_Image;
}

glm::vec4 Renderer::PixelProgram(int i, int j) const noexcept {
    Ray ray;
    ray.origin = m_Camera.GetPosition();
    ray.direction = m_Camera.GetRayDirections()[m_Width * i + j];

    glm::vec3 color(0.f);
    glm::vec3 contribution(1.f);
    for (int i = 0; i < m_MaxRayDepth; ++i) {
        HitPayload payload = TraceRay(ray);

        if (payload.t < 0.f) {
            glm::vec3 skyColor(0.6f, 0.7f, 0.9f);
            color += skyColor * contribution;
            break;
        }

        // contribution *= ...; todo!
    }

    return {color, 1.f};
}

HitPayload Renderer::TraceRay(const Ray &ray) const noexcept {
    int objectIndex = -1;
    float t = std::numeric_limits<float>::max();
    // todo!

    if (objectIndex < 0) {
        return Miss(ray);
    }

    return ClosestHit(ray, t, objectIndex);
}

HitPayload Renderer::ClosestHit(const Ray &ray, float t, int objectIndex) const noexcept {
    // todo!
}

HitPayload Renderer::Miss(const Ray &ray) const noexcept {
    HitPayload payload;
    payload.t = -1.f;
    return payload;
}