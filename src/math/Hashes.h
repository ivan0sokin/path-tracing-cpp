#ifndef _HASHES_H
#define _HASHES_H

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include <functional>

namespace std {
    template<typename T>
    struct hash<Math::Types::Vector<T, 2>> {
        size_t operator()(const Math::Types::Vector<T, 2> &v) const noexcept {
            auto h1 = hash<T>{}(v.x);
            auto h2 = hash<T>{}(v.y);
            return h1 ^ (h2 << 1);
        }
    };

    template<typename T>
    struct hash<Math::Types::Vector<T, 3>> {
        size_t operator()(const Math::Types::Vector<T, 3> &v) const noexcept {
            auto h1 = hash<Math::Types::Vector<T, 2>>{}(Math::Types::Vector<T, 2>(v.x, v.y));
            auto h2 = hash<T>{}(v.z);
            return h1 ^ (h2 << 1);
        }
    };

    template<typename T>
    struct hash<Math::Types::Vector<T, 4>> {
        size_t operator()(const Math::Types::Vector<T, 4> &v) const noexcept {
            auto h1 = hash<Math::Types::Vector<T, 3>>{}(Math::Types::Vector<T, 2>(v.x, v.y, v.z));
            auto h2 = hash<T>{}(v.w);
            return h1 ^ (h2 << 1);
        }
    };
}


#endif