#include "Image.h"
#include <gl/gl.h>

Image::Image(int width, int height) noexcept :
    m_Data(new std::uint32_t[width * height]), m_Width(width), m_Height(height) {
    
    glGenTextures(1, &m_Descriptor);
    glBindTexture(GL_TEXTURE_2D, m_Descriptor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)m_Data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Image::~Image() noexcept {
    if (m_Data != nullptr) {
        delete m_Data;
    }

    glDeleteTextures(1, &m_Descriptor);
}

void Image::SetPixel(int index, std::uint32_t value) noexcept {
    m_Data[index] = value;
}

void Image::Update() noexcept {
    glBindTexture(GL_TEXTURE_2D, m_Descriptor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, (void*)m_Data);

    glBindTexture(GL_TEXTURE_2D, 0);
}