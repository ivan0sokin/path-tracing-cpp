#include "Image.h"
#include <gl/gl.h>

Image::Image(int width, int height, const uint32_t *data) noexcept :
    m_Width(width), m_Height(height) {
    glGenTextures(1, &m_Descriptor);
    glBindTexture(GL_TEXTURE_2D, m_Descriptor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Image::~Image() noexcept {
    glDeleteTextures(1, &m_Descriptor);
}

void Image::UpdateData(const uint32_t *data) noexcept {
    glBindTexture(GL_TEXTURE_2D, m_Descriptor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data);

    glBindTexture(GL_TEXTURE_2D, 0);
}