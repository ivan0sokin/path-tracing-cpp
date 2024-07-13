#include "ImageSaver.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb-master/stb_image_write.h"

void ImageSaver::Save(const std::filesystem::path &pathToFile) noexcept {
    if (m_Image == nullptr) {
        return;
    }

    stbi_write_png(
        pathToFile.string().c_str(),
        m_Image->GetWidth(),
        m_Image->GetHeight(),
        m_Image->GetComponentCount(),
        (const void*)m_Image->GetData(),
        m_Image->GetStrideInBytes()
    );
}