#ifndef _IMAGE_SAVER_H
#define _IMAGE_SAVER_H

#include "Image.h"

#include <filesystem>

class ImageSaver {
public:
    constexpr ImageSaver() noexcept :
        m_Image(nullptr) {}

    constexpr ImageSaver(const Image *image) noexcept :
        m_Image(image) {}
    
    void Save(const std::filesystem::path &pathToFile) noexcept;

private:
    const Image *m_Image;
};

#endif