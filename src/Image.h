#ifndef _IMAGE_H
#define _IMAGE_H

#include <vector>
#include <cstdint>

//! OpenGL image
class Image {
public:
    Image() = delete;

    //! Creates new image with RGBA ```data```
    Image(int width, int height, const uint32_t *data) noexcept;

    ~Image() noexcept;

    //! Updates image with same size using new ```data```
    void UpdateData(const uint32_t *data) noexcept;

    //! Returns OpenGL descriptor to texture
    constexpr unsigned int GetDescriptor() const noexcept {
        return m_Descriptor;
    }
    
private:
    int m_Width, m_Height;
    unsigned int m_Descriptor;
};

#endif