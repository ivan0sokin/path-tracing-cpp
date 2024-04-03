#ifndef _IMAGE_H
#define _IMAGE_H

#include <vector>
#include <cstdint>

class Image {
public:
    Image() = delete;

    Image(int width, int height, const uint32_t *data) noexcept;

    ~Image() noexcept;

    void UpdateData(const uint32_t *data) noexcept;

    constexpr unsigned int GetDescriptor() const noexcept {
        return m_Descriptor;
    }
private:
    int m_Width, m_Height;
    unsigned int m_Descriptor;
};

#endif