#ifndef _IMAGE_SAVER_H
#define _IMAGE_SAVER_H

#include "Image.h"

#include <filesystem>

//! Class that saves image
class ImageSaver {
public:
    //! Constructs ImageSaver with nothing
    constexpr ImageSaver() noexcept :
        m_Image(nullptr) {}

    //! Constructs ImageSaver with given pointer to Image
    constexpr ImageSaver(const Image *image) noexcept :
        m_Image(image) {}
    
    //! Saves Image to file with given path
    void Save(const std::filesystem::path &pathToFile) noexcept;

private:
    const Image *m_Image;
};

#endif