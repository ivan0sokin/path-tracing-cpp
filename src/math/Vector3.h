#ifndef _VECTOR_3_H
#define _VECTOR_3_H

#include "Types.h"

namespace Math {
    namespace Types {
        template<typename T>
        struct Vector<T, 3> {
            union {
                struct { T x, y, z; };
                struct { T r, g, b; };
                struct { T data[3]; };
            };

            constexpr Vector() noexcept :
                x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)) {}

            explicit constexpr Vector(T scalar) noexcept :
                x(scalar), y(scalar), z(scalar) {}

            constexpr Vector(T x, T y, T z) noexcept :
                x(x), y(y), z(z) {}

            constexpr Vector(const Vector<T, 4> &v) noexcept :
                x(v.x), y(v.y), z(v.z) {}

            constexpr T operator[](std::size_t index) const noexcept {
                return data[index];
            }

            constexpr T& operator[](std::size_t index) noexcept {
                return data[index];
            }
        };

        template<typename T>
        constexpr bool operator==(const Vector<T, 3> &a, const Vector<T, 3> &b) noexcept {
            return a.x == b.x && a.y == b.y && a.z == b.z;
        }

        template<typename T>
        constexpr Vector<T, 3> operator+(const Vector<T, 3> &a, const Vector<T, 3> &b) noexcept {
            return {
                a.x + b.x,
                a.y + b.y,
                a.z + b.z
            };
        }

        template<typename T>
        constexpr Vector<T, 3> operator-(const Vector<T, 3> &a, const Vector<T, 3> &b) noexcept {
            return {
                a.x - b.x,
                a.y - b.y,
                a.z - b.z
            };
        }

        template<typename T>
        constexpr Vector<T, 3> operator*(const Vector<T, 3> &a, const Vector<T, 3> &b) noexcept {
            return {
                a.x * b.x,
                a.y * b.y,
                a.z * b.z
            };
        }

        template<typename T>
        constexpr Vector<T, 3>& operator+=(Vector<T, 3> &a, const Vector<T, 3> &b) noexcept {
            a.x += b.x;
            a.y += b.y;
            a.z += b.z;
            return a;
        }

        template<typename T>
        constexpr Vector<T, 3>& operator-=(Vector<T, 3> &a, const Vector<T, 3> &b) noexcept {
            a.x -= b.x;
            a.y -= b.y;
            a.z -= b.z;
            return a;
        }

        template<typename T>
        constexpr Vector<T, 3>& operator*=(Vector<T, 3> &a, const Vector<T, 3> &b) noexcept {
            a.x *= b.x;
            a.y *= b.y;
            a.z *= b.z;
            return a;
        }

        template<typename T>
        constexpr Vector<T, 3> operator-(const Vector<T, 3> &v) noexcept {
            return {
                -v.x,
                -v.y,
                -v.z
            };
        }

        template<typename T>
        constexpr Vector<T, 3> operator+(const Vector<T, 3> &v, float scalar) noexcept {
            return {
                v.x + scalar,
                v.y + scalar,
                v.z + scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 3> operator-(const Vector<T, 3> &v, float scalar) noexcept {
            return {
                v.x - scalar,
                v.y - scalar,
                v.z - scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 3> operator*(const Vector<T, 3> &v, float scalar) noexcept {
            return {
                v.x * scalar,
                v.y * scalar,
                v.z * scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 3> operator/(const Vector<T, 3> &v, float scalar) noexcept {
            return {
                v.x / scalar,
                v.y / scalar,
                v.z / scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 3>& operator+=(Vector<T, 3> &v, float scalar) noexcept {
            v.x += scalar;
            v.y += scalar;
            v.z += scalar;
            return v;
        }

        template<typename T>
        constexpr Vector<T, 3>& operator-=(Vector<T, 3> &v, float scalar) noexcept {
            v.x -= scalar;
            v.y -= scalar;
            v.z -= scalar;
            return v;
        }

        template<typename T>
        constexpr Vector<T, 3>& operator*=(Vector<T, 3> &v, float scalar) noexcept {
            v.x *= scalar;
            v.y *= scalar;
            v.z *= scalar;
            return v;
        }

        template<typename T>
        constexpr Vector<T, 3>& operator/=(Vector<T, 3> &v, float scalar) noexcept {
            v.x /= scalar;
            v.y /= scalar;
            v.z /= scalar;
            return v;
        }

        template<typename T>
        constexpr Vector<T, 3> operator+(float scalar, const Vector<T, 3> &v) noexcept {
            return {
                v.x + scalar,
                v.y + scalar,
                v.z + scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 3> operator-(float scalar, const Vector<T, 3> &v) noexcept {
            return {
                scalar - v.x,
                scalar - v.y,
                scalar - v.z
            };
        }

        template<typename T>
        constexpr Vector<T, 3> operator*(float scalar, const Vector<T, 3> &v) noexcept {
            return {
                v.x * scalar,
                v.y * scalar,
                v.z * scalar
            };
        }

        template<typename T>
        constexpr Vector<T, 3> operator/(float scalar, const Vector<T, 3> &v) noexcept {
            return {
                scalar / v.x,
                scalar / v.y,
                scalar / v.z
            };
        }
    }
}

#endif