#ifndef _GEOMETRIC_FUNCTIONS_H
#define _GEOMETRIC_FUNCTIONS_H

#include "Types.h"
#include "ElementaryFunctions.h"

#include <span>
#include <utility>

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
    constexpr T Angle(const Types::Vector<T, 2> &a, const Types::Vector<T, 2> &b) noexcept {
        return std::atan2(Math::Cross(a, b), Math::Dot(a, b));
    }

    template<typename T>
    constexpr T Angle(const Types::Vector<T, 3> &a, const Types::Vector<T, 3> &b) noexcept {
        return std::atan2(Math::Length(Math::Cross(a, b)), Math::Dot(a, b));
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

    template<typename T>
    constexpr Types::Vector<T, 3> Reflect(const Types::Vector<T, 3> &i, const Types::Vector<T, 3> &n) noexcept {
        return i - static_cast<T>(2) * Math::Dot(n, i) * n;
    }

    template<typename T>
    constexpr Types::Vector<T, 3> Refract(const Types::Vector<T, 3> &i, const Types::Vector<T, 3> &n, float eta) noexcept {
        float k = static_cast<T>(1) - eta * eta * (static_cast<T>(1) - Math::Dot(n, i) * Math::Dot(n, i));
        if (k < static_cast<T>(0)) {
            return Types::Vector<T, 3>(0.f);
        }

        return eta * i - (eta * Math::Dot(n, i) + Math::Sqrt(k)) * n;
    }
}

#endif