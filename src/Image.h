#ifndef _IMAGE_H
#define _IMAGE_H

#include <vector>
#include <cstdint>

class Image {
public:
    Image() = delete;

    Image(int width, int height, const uint32_t *data) noexcept;

    ~Image() noexcept;

    unsigned int GetDescriptor() const noexcept {
        return m_Descriptor;
    }
private:
    unsigned int m_Descriptor;
};

#endif