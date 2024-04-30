#ifndef _VECTOR_4_H
#define _VECTOR_4_H

#include "Vector.h"

namespace Math {
    namespace Types {
        template<typename T>
        struct Vector<T, 4> {
            union {
                struct { T x, y, z, w; };
                struct { T r, g, b, a; };
            };

            constexpr Vector() noexcept :
                x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)), w(static_cast<T>(0)) {}

            explicit constexpr Vector(T scalar) noexcept :
                x(scalar), y(scalar), z(scalar), w(scalar) {}

            constexpr Vector(T x, T y, T z, T w) noexcept :
                x(x), y(y), z(z), w(w) {}
        };

        
        template<typename T>
        constexpr bool operator==(const Vector<T, 4> &a, const Vector<T, 4> &b) noexcept {
            return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
        }

        template<typename T>
        constexpr Vector<T, 4> operator+(const Vector<T, 4> &a, const Vector<T, 4> &b) noexcept {
            return {
                a.x + b.x,
                a.y + b.y,
                a.z + b.z,
                a.w + b.w
            };
        }

        template<typename T>
        constexpr Vector<T, 4> operator-(const Vector<T, 4> &a, const Vector<T, 4> &b) noexcept {
            return {
                a.x - b.x,
                a.y - b.y,
                a.z - b.z,
                a.w - b.w
            };
        }

        template<typename T>
        constexpr Vector<T, 4> operator*(const Vector<T, 4> &a, const Vector<T, 4> &b) noexcept {
            return {
                a.x * b.x,
                a.y * b.y,
                a.z * b.z,
                a.w * b.w
            };
        }

        template<typename T>
        constexpr Vector<T, 4>& operator+=(Vector<T, 4> &a, const Vector<T, 4> &b) noexcept {
            a.x += b.x;
            a.y += b.y;
            a.z += b.z;
            a.w += b.w;
            return a;
        }

        template<typename T>
        constexpr Vector<T, 4>& operator-=(Vector<T, 4> &a, const Vector<T, 4> &b) noexcept {
            a.x -= b.x;
            a.y -= b.y;
            a.z -= b.z;
            a.w -= b.w;
            return a;
        }

        template<typename T>
        constexpr Vector<T, 4>& operator*=(Vector<T, 4> &a, const Vector<T, 4> &b) noexcept {
            a.x *= b.x;
            a.y *= b.y;
            a.z *= b.z;
            a.w *= b.w;
            return a;
        }

        template<typename T>
        constexpr Vector<T, 4> operator-(const Vector<T, 4> &v) noexcept {
            return {
                -v.x,
                -v.y,
                -v.z,
                -v.w
            };
        }

        template<typename T>
        constexpr Vector<T, 4> operator+(const Vector<T, 4> &v, float scalar) noexcept {
            return {
                v.x + scalar,
                v.y + scalar,
                v.z + scalar,
                v.w + scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 4> operator-(const Vector<T, 4> &v, float scalar) noexcept {
            return {
                v.x - scalar,
                v.y - scalar,
                v.z - scalar,
                v.w - scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 4> operator*(const Vector<T, 4> &v, float scalar) noexcept {
            return {
                v.x * scalar,
                v.y * scalar,
                v.z * scalar,
                v.w * scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 4> operator/(const Vector<T, 4> &v, float scalar) noexcept {
            return {
                v.x / scalar,
                v.y / scalar,
                v.z / scalar,
                v.w / scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 4>& operator+=(Vector<T, 4> &v, float scalar) noexcept {
            v.x += scalar;
            v.y += scalar;
            v.z += scalar;
            v.w += scalar;
            return v;
        }

        template<typename T>
        constexpr Vector<T, 4>& operator-=(Vector<T, 4> &v, float scalar) noexcept {
            v.x -= scalar;
            v.y -= scalar;
            v.z -= scalar;
            v.w -= scalar;
            return v;
        }

        template<typename T>
        constexpr Vector<T, 4>& operator*=(Vector<T, 4> &v, float scalar) noexcept {
            v.x *= scalar;
            v.y *= scalar;
            v.z *= scalar;
            v.w *= scalar;
            return v;
        }

        template<typename T>
        constexpr Vector<T, 4>& operator/=(Vector<T, 4> &v, float scalar) noexcept {
            v.x /= scalar;
            v.y /= scalar;
            v.z /= scalar;
            v.w /= scalar;
            return v;
        }

        template<typename T>
        constexpr Vector<T, 4> operator+(float scalar, const Vector<T, 4> &v) noexcept {
            return {
                v.x + scalar,
                v.y + scalar,
                v.z + scalar,
                v.w + scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 4> operator-(float scalar, const Vector<T, 4> &v) noexcept {
            return {
                scalar - v.x,
                scalar - v.y,
                scalar - v.z,
                scalar - v.w
            };
        }

        template<typename T>
        constexpr Vector<T, 4> operator*(float scalar, const Vector<T, 4> &v) noexcept {
            return {
                v.x * scalar,
                v.y * scalar,
                v.z * scalar,
                v.w * scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 4> operator/(float scalar, const Vector<T, 4> &v) noexcept {
            return {
                scalar / v.x,
                scalar / v.y,
                scalar / v.z,
                scalar / v.w
            };
        }
    }
}

#endif