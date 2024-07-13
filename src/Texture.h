#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "math/Math.h"

#include <vector>
#include <span>

class Texture {
public:
    inline Texture(const Math::Vector3f &color) noexcept :
        m_Data(1, color), m_Width(1), m_Height(1) {}

    inline Texture() noexcept :
        Texture({1.f, 1.f, 1.f}) {}

    inline Texture(const unsigned char *dataInBytes, int width, int height, int channels) noexcept :
        m_Width(width), m_Height(height) {
        int texelCount = width * height;
        m_Data.resize(texelCount);

        const float inverseByteMax = 1.f / 255.f;
        for (int i = 0; i < texelCount; ++i) {
            float r = static_cast<float>(dataInBytes[channels * i + 0]);
            float g = static_cast<float>(dataInBytes[channels * i + 1]);
            float b = static_cast<float>(dataInBytes[channels * i + 2]);
            m_Data[i] = {r * inverseByteMax, g * inverseByteMax, b * inverseByteMax};
        }
    }

    inline ~Texture() noexcept = default;

    inline Math::Vector3f PickValue(const Math::Vector2f &texcoord) const noexcept {
        int i = static_cast<int>(static_cast<float>(m_Height - 1) * texcoord.v);
        int j = static_cast<int>(static_cast<float>(m_Width - 1) * texcoord.u);
        return m_Data[m_Width * i + j];
    }

    inline float* GetData() noexcept {
        return &m_Data[0].r;
    }

    inline const float* GetData() const noexcept {
        return &m_Data[0].r;
    }

    constexpr int GetTexelCount() const noexcept {
        return m_Width * m_Height;
    }

private:
    std::vector<Math::Vector3f> m_Data;
    int m_Width, m_Height;
};

#endif