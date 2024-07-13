#ifndef _IMAGE_H
#define _IMAGE_H

#include <vector>
#include <cstdint>

//! OpenGL image
class Image {
public:
    Image() = delete;

    //! Creates new image with RGBA ```data```
    Image(int width, int height) noexcept;

    ~Image() noexcept;

    void SetPixel(int index, std::uint32_t value) noexcept;

    //! Updates image with same size using new ```data```
    void Update() noexcept;

    //! Returns OpenGL descriptor to texture
    constexpr unsigned int GetDescriptor() const noexcept {
        return m_Descriptor;
    }

    constexpr int GetWidth() const noexcept {
        return m_Width;
    }

    constexpr int GetHeight() const noexcept {
        return m_Height;
    }

    constexpr const std::uint32_t* GetData() const noexcept {
        return m_Data;
    }

    constexpr int GetComponentCount() const noexcept {
        return 4;
    }

    constexpr int GetStrideInBytes() const noexcept {
        return m_Width * sizeof(m_Data[0]);
    }
    
private:
    std::uint32_t *m_Data; 
    int m_Width, m_Height;
    unsigned int m_Descriptor;
};

#endif