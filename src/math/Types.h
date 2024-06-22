#ifndef _TYPES_H
#define _TYPES_H

#include <cstddef>

namespace Math {
    namespace Types {
        template<typename T, std::size_t N>
        struct Vector;

        template<typename T, std::size_t R, std::size_t C>
        struct Matrix;
    }
}

#endif