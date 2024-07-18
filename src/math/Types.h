#ifndef _TYPES_H
#define _TYPES_H

#include <cstddef>

namespace Math {
    //! Internal namespace for abstact math structs
    namespace Types {
        //! ```N```-dimensional Vector
        template<typename T, std::size_t N>
        struct Vector;

        //! Matrix with ```R``` rows and ```C``` columns
        template<typename T, std::size_t R, std::size_t C>
        struct Matrix;
    }
}

#endif