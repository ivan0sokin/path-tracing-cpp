#ifndef _IMAGE_H
#define _IMAGE_H

#include <vector>
#include <cstdint>

//! RGBA image
class Image {
public:
    Image() = delete;

    //! Creates new image with given size
    Image(int width, int height) noexcept;

    ~Image() noexcept;

    //! Sets RGBA value at element with given  ```index```
    void SetPixel(int index, std::uint32_t value) noexcept;

    //! Updates image information in GPU
    void Update() noexcept;

    //! Returns descriptor to texture
    constexpr unsigned int GetDescriptor() const noexcept {
        return m_Descriptor;
    }

    //! Returns width of Image
    constexpr int GetWidth() const noexcept {
        return m_Width;
    }

    //! Returns height of Image
    constexpr int GetHeight() const noexcept {
        return m_Height;
    }

    //! Returns pointer to RGBA data
    constexpr const std::uint32_t* GetData() const noexcept {
        return m_Data;
    }

    //! Returns number of channels in color. Returns 4
    constexpr int GetComponentCount() const noexcept {
        return 4;
    }
    
    //! Returns size of one line in bytes
    constexpr int GetStrideInBytes() const noexcept {
        return m_Width * sizeof(m_Data[0]);
    }
    
private:
    std::uint32_t *m_Data; 
    int m_Width, m_Height;
    unsigned int m_Descriptor;
};

#endif