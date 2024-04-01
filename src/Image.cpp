#include "Image.h"
#include <gl/gl.h>

Image::Image(int width, int height, const uint32_t *data) noexcept {
    glGenTextures(1, &m_Descriptor);
    glBindTexture(GL_TEXTURE_2D, m_Descriptor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

Image::~Image() noexcept {
    glDeleteTextures(1, &m_Descriptor);
}