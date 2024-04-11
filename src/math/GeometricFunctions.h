#ifndef _GEOMETRIC_FUNCTIONS_H
#define _GEOMETRIC_FUNCTIONS_H

#include "Vector.h"
#include "ElementaryFunctions.h"

namespace Math {
    template<typename T>
    constexpr T Dot(const Types::Vector<T, 2> &a, const Types::Vector<T, 2> &b) noexcept {
        return a.x * b.x + a.y * b.y;
    }

    template<typename T>
    constexpr T Dot(const Types::Vector<T, 3> &a, const Types::Vector<T, 3> &b) noexcept {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    template<typename T>
    constexpr T Dot(const Types::Vector<T, 4> &a, const Types::Vector<T, 4> &b) noexcept {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    template<typename T>
    constexpr T Cross(const Types::Vector<T, 2> &a, const Types::Vector<T, 2> &b) noexcept {
        return a.x * b.y - a.y * b.x;
    }

    template<typename T>
    constexpr Types::Vector<T, 3> Cross(const Types::Vector<T, 3> &a, const Types::Vector<T, 3> &b) noexcept {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    template<typename T>
    constexpr T Length(const Types::Vector<T, 2> &v) noexcept {
        return Sqrt(Dot(v, v));
    }

    template<typename T>
    constexpr T Length(const Types::Vector<T, 3> &v) noexcept {
        return Sqrt(Dot(v, v));
    }

    template<typename T>
    constexpr T Length(const Types::Vector<T, 4> &v) noexcept {
        return Sqrt(Dot(v, v));
    }

    template<typename T>
    constexpr Types::Vector<T, 2> Normalize(const Types::Vector<T, 2> &v) noexcept {
        T inverseLength = static_cast<T>(1) / Length(v); 
        return v * inverseLength;
    }

    template<typename T>
    constexpr Types::Vector<T, 3> Normalize(const Types::Vector<T, 3> &v) noexcept {
        T inverseLength = static_cast<T>(1) / Length(v); 
        return v * inverseLength;
    }

    template<typename T>
    constexpr Types::Vector<T, 4> Normalize(const Types::Vector<T, 4> &v) noexcept {
        T inverseLength = static_cast<T>(1) / Length(v); 
        return v * inverseLength;
    }
}

#endif