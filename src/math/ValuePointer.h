#ifndef _VALUE_POINTER_H
#define _VALUE_POINTER_H

#include "Types.h"

namespace Math {
    template<typename T>
    constexpr T* ValuePointer(T &v) noexcept {
        return &v;
    }

    template<typename T>
    constexpr T* ValuePointer(Types::Vector<T, 2> &v) noexcept {
        return &v.x;
    }

    template<typename T>
    constexpr T* ValuePointer(Types::Vector<T, 3> &v) noexcept {
        return &v.x;
    }

    template<typename T>
    constexpr T* ValuePointer(Types::Vector<T, 4> &v) noexcept {
        return &v.x;
    }
}

#endif