#ifndef _ELEMENTARY_FUNCTIONS_H
#define _ELEMENTARY_FUNCTIONS_H

#include "Types.h"

#include <cmath>

namespace Math {
    template<typename T>
    constexpr T Abs(T value) noexcept {
        return std::abs(value);
    }

    template<typename T>
    constexpr T Min(T a, T b) noexcept {
        return a < b ? a : b;
    }

    template<typename T>
    constexpr Types::Vector<T, 2> Min(const Types::Vector<T, 2> &a, const Types::Vector<T, 2> &b) noexcept {
        return {
            Min(a.x, b.x),
            Min(a.y, b.y)
        };
    }

    template<typename T>
    constexpr Types::Vector<T, 3> Min(const Types::Vector<T, 3> &a, const Types::Vector<T, 3> &b) noexcept {
        return {
            Min(a.x, b.x),
            Min(a.y, b.y),
            Min(a.z, b.z)
        };
    }

    template<typename T>
    constexpr Types::Vector<T, 4> Min(const Types::Vector<T, 4> &a, const Types::Vector<T, 4> &b) noexcept {
        return {
            Min(a.x, b.x),
            Min(a.y, b.y),
            Min(a.z, b.z),
            Min(a.w, b.w)
        };
    }

    template<typename T>
    constexpr T Max(T a, T b) noexcept {
        return a > b ? a : b;
    }

    template<typename T>
    constexpr Types::Vector<T, 2> Max(const Types::Vector<T, 2> &a, const Types::Vector<T, 2> &b) noexcept {
        return {
            Max(a.x, b.x),
            Max(a.y, b.y)
        };
    }

    template<typename T>
    constexpr Types::Vector<T, 3> Max(const Types::Vector<T, 3> &a, const Types::Vector<T, 3> &b) noexcept {
        return {
            Max(a.x, b.x),
            Max(a.y, b.y),
            Max(a.z, b.z)
        };
    }

    template<typename T>
    constexpr Types::Vector<T, 4> Max(const Types::Vector<T, 4> &a, const Types::Vector<T, 4> &b) noexcept {
        return {
            Max(a.x, b.x),
            Max(a.y, b.y),
            Max(a.z, b.z),
            Max(a.w, b.w)
        };
    }

    template<typename T>
    constexpr T MinComponent(const Types::Vector<T, 2> &v) noexcept {
        return Min(v.x, v.y);
    }

    template<typename T>
    constexpr T MinComponent(const Types::Vector<T, 3> &v) noexcept {
        return Min(Min(v.x, v.y), v.z);
    }

    template<typename T>
    constexpr T MinComponent(const Types::Vector<T, 4> &v) noexcept {
        return Min(Min(v.x, v.y), Min(v.z, v.w));
    }

    template<typename T>
    constexpr T MaxComponent(const Types::Vector<T, 2> &v) noexcept {
        return Max(v.x, v.y);
    }

    template<typename T>
    constexpr T MaxComponent(const Types::Vector<T, 3> &v) noexcept {
        return Max(Max(v.x, v.y), v.z);
    }

    template<typename T>
    constexpr T MaxComponent(const Types::Vector<T, 4> &v) noexcept {
        return Max(Max(v.x, v.y), Max(v.z, v.w));
    }

    template<typename T>
    constexpr T Clamp(T value, T min, T max) noexcept {
        return Max(min, Min(max, value));
    }

    template<typename T>
    constexpr Types::Vector<T, 2> Clamp(const Types::Vector<T, 2> &v, float min, float max) noexcept {
        return {
            Clamp(v.x, min, max),
            Clamp(v.y, min, max)
        };
    }

    template<typename T>
    constexpr Types::Vector<T, 3> Clamp(const Types::Vector<T, 3> &v, float min, float max) noexcept {
        return {
            Clamp(v.x, min, max),
            Clamp(v.y, min, max),
            Clamp(v.z, min, max)
        };
    }

    template<typename T>
    constexpr Types::Vector<T, 4> Clamp(const Types::Vector<T, 4> &v, float min, float max) noexcept {
        return {
            Clamp(v.x, min, max),
            Clamp(v.y, min, max),
            Clamp(v.z, min, max),
            Clamp(v.w, min, max)
        };
    }

    template<typename T>
    constexpr Types::Vector<T, 2> Clamp(const Types::Vector<T, 2> &v, const Types::Vector<T, 2> &min, const Types::Vector<T, 2> &max) noexcept {
        return {
            Clamp(v.x, min.x, max.x),
            Clamp(v.y, min.y, max.y)
        };
    }

    template<typename T>
    constexpr Types::Vector<T, 3> Clamp(const Types::Vector<T, 3> &v, const Types::Vector<T, 3> &min, const Types::Vector<T, 2> &max) noexcept {
        return {
            Clamp(v.x, min.x, max.x),
            Clamp(v.y, min.y, max.y),
            Clamp(v.y, min.z, max.z)
        };
    }

    template<typename T>
    constexpr Types::Vector<T, 4> Clamp(const Types::Vector<T, 4> &v, const Types::Vector<T, 4> &min, const Types::Vector<T, 2> &max) noexcept {
        return {
            Clamp(v.x, min.x, max.x),
            Clamp(v.y, min.y, max.y),
            Clamp(v.y, min.z, max.z),
            Clamp(v.w, min.w, max.w)
        };
    }

    template<typename T>
    constexpr T Lerp(T a, T b, T t) noexcept {
        return a * (static_cast<T>(1) - t) + b * t;
    }

    template<typename T>
    constexpr Types::Vector<T, 2> Lerp(const Types::Vector<T, 2> &a, const Types::Vector<T, 2> &b, T t) noexcept {
        return a * (static_cast<T>(1) - t) + b * t;
    }

    template<typename T>
    constexpr Types::Vector<T, 3> Lerp(const Types::Vector<T, 3> &a, const Types::Vector<T, 3> &b, T t) noexcept {
        return a * (static_cast<T>(1) - t) + b * t;
    }

    template<typename T>
    constexpr Types::Vector<T, 4> Lerp(const Types::Vector<T, 4> &a, const Types::Vector<T, 4> &b, T t) noexcept {
        return a * (static_cast<T>(1) - t) + b * t;
    }
    
    template<typename T>
    constexpr T Sqrt(T value) noexcept {
        return std::sqrt(value);
    }

    template<typename T>
    constexpr T Pow(T base, T exponent) noexcept {
        return std::pow(base, exponent);
    }
}

#endif