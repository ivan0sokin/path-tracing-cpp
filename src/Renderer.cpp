#include "Renderer.h"
#include "Utilities.hpp"

#include <stdio.h> 

Image* Renderer::Render() noexcept {
    if (m_Image != nullptr) {
        return m_Image;
    }

    m_ImageData = new uint32_t[m_Width * m_Height];

    for (int i = 0; i < m_Height; ++i) {
        for (int j = 0; j < m_Width; ++j) {
            m_ImageData[m_Width * i + j] = Utilities::ConvertColorToRGBA(PixelProgram(j, i));
        }
    }

    m_Image = new Image(m_Width, m_Height, m_ImageData);
    return m_Image;
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

glm::vec4 Renderer::PixelProgram(int x, int y) noexcept {
    return {1.f, 0.f, 0.f, 1.f};
}