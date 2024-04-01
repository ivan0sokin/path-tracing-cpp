#ifndef _UTILITIES_HPP
#define _UTILITIES_HPP

#include <cstdio>

#include "../glm/include/glm/vec4.hpp"

namespace Utilities {
    constexpr uint32_t ConvertColorToRGBA(glm::vec4 color) {
        uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		return (a << 24) | (b << 16) | (g << 8) | r;
    }
}

#endif