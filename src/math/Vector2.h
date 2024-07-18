#ifndef _VECTOR_2_H
#define _VECTOR_2_H

#include "Types.h"

namespace Math {
    namespace Types {
        //! 2D Vector
        template<typename T>
        struct Vector<T, 2> {
            union {
                struct { T x, y; };
                struct { T u, v; };
                struct { T data[2]; };
            };

            constexpr Vector() noexcept :
                x(static_cast<T>(0)), y(static_cast<T>(0)) {}

            explicit constexpr Vector(T scalar) noexcept :
                x(scalar), y(scalar) {}

            constexpr Vector(T x, T y) noexcept :
                x(x), y(y) {}

            constexpr T operator[](std::size_t index) const noexcept {
                return data[index];
            }

            constexpr T& operator[](std::size_t index) noexcept {
                return data[index];
            }
        };

        template<typename T>
        constexpr bool operator==(const Vector<T, 2> &a, const Vector<T, 2> &b) noexcept {
            return a.x == b.x && a.y == b.y;
        }

        template<typename T>
        constexpr Vector<T, 2> operator+(const Vector<T, 2> &a, const Vector<T, 2> &b) noexcept {
            return {
                a.x + b.x,
                a.y + b.y
            };
        }

        template<typename T>
        constexpr Vector<T, 2> operator-(const Vector<T, 2> &a, const Vector<T, 2> &b) noexcept {
            return {
                a.x - b.x,
                a.y - b.y
            };
        }

        template<typename T>
        constexpr Vector<T, 2> operator*(const Vector<T, 2> &a, const Vector<T, 2> &b) noexcept {
            return {
                a.x * b.x,
                a.y * b.y
            };
        }

        template<typename T>
        constexpr Vector<T, 2>& operator+=(Vector<T, 2> &a, const Vector<T, 2> &b) noexcept {
            a.x += b.x;
            a.y += b.y;
            return a;
        }

        template<typename T>
        constexpr Vector<T, 2>& operator-=(Vector<T, 2> &a, const Vector<T, 2> &b) noexcept {
            a.x -= b.x;
            a.y -= b.y;
            return a;
        }

        template<typename T>
        constexpr Vector<T, 2>& operator*=(Vector<T, 2> &a, const Vector<T, 2> &b) noexcept {
            a.x *= b.x;
            a.y *= b.y;
            return a;
        }

        template<typename T>
        constexpr Vector<T, 2> operator-(const Vector<T, 2> &v) noexcept {
            return {
                -v.x,
                -v.y
            };
        }

        template<typename T>
        constexpr Vector<T, 2> operator+(const Vector<T, 2> &v, float scalar) noexcept {
            return {
                v.x + scalar,
                v.y + scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 2> operator-(const Vector<T, 2> &v, float scalar) noexcept {
            return {
                v.x - scalar,
                v.y - scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 2> operator*(const Vector<T, 2> &v, float scalar) noexcept {
            return {
                v.x * scalar,
                v.y * scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 2> operator/(const Vector<T, 2> &v, float scalar) noexcept {
            return {
                v.x / scalar,
                v.y / scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 2>& operator+=(Vector<T, 2> &v, float scalar) noexcept {
            v.x += scalar;
            v.y += scalar;
            return v;
        }

        template<typename T>
        constexpr Vector<T, 2>& operator-=(Vector<T, 2> &v, float scalar) noexcept {
            v.x -= scalar;
            v.y -= scalar;
            return v;
        }

        template<typename T>
        constexpr Vector<T, 2>& operator*=(Vector<T, 2> &v, float scalar) noexcept {
            v.x *= scalar;
            v.y *= scalar;
            return v;
        }

        template<typename T>
        constexpr Vector<T, 2>& operator/=(Vector<T, 2> &v, float scalar) noexcept {
            v.x /= scalar;
            v.y /= scalar;
            return v;
        }

        template<typename T>
        constexpr Vector<T, 2> operator*(float scalar, const Vector<T, 2> &v) noexcept {
            return {
                v.x * scalar,
                v.y * scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 2> operator/(float scalar, const Vector<T, 2> &v) noexcept {
            return {
                scalar / v.x,
                scalar / v.y
            };
        }

        template<typename T>
        constexpr Vector<T, 2> operator+(float scalar, const Vector<T, 2> &v) noexcept {
            return {
                v.x + scalar,
                v.y + scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 2> operator-(float scalar, const Vector<T, 2> &v) noexcept {
            return {
                scalar - v.x,
                scalar - v.y
            };
        }
    }
}

#endif