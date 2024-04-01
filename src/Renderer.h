#ifndef _RENDERER_H
#define _RENDERER_H

#include "../glm/include/glm/vec4.hpp"

#include "Image.h"

class Renderer {
public:
    Renderer() = delete;
    
    constexpr Renderer(int width, int height) noexcept : m_Width(width), m_Height(height) {}

    Image* Render() noexcept;

    void OnResize(int width, int height) noexcept;
private:
    glm::vec4 PixelProgram(int x, int y) noexcept;

private:
    int m_Width, m_Height;
    Image *m_Image = nullptr;
    uint32_t *m_ImageData = nullptr;
};

#endif