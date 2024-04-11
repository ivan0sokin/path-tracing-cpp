#ifndef _TRIGONOMETRIC_FUNCTIONS_H
#define _TRIGONOMETRIC_FUNCTIONS_H

#include "Constants.h"

#include <cmath>

namespace Math {
    template<typename T>
    constexpr T ToRadians(T degrees) noexcept {
        return degrees * Constants::HalfCircumferenceInRadians<T> * Constants::InverseHalfCircumferenceInDegrees<T>;
    }

    template<typename T>
    constexpr T ToDegrees(T radians) noexcept {
        return radians * Constants::HalfCircumferenceInDegrees<T> * Constants::InverseHalfCircumferenceInRadians<T>;
    }

    template<typename T>
    constexpr T Sin(T value) noexcept {
        return std::sin(value);
    }

    template<typename T>
    constexpr T Cos(T value) noexcept {
        return std::cos(value);
    }

    template<typename T>
    constexpr T Tan(T value) noexcept {
        return std::tan(value);
    }
}

#endif