#ifndef _MATH_H
#define _MATH_H

#include "Vector.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "ElementaryFunctions.h"
#include "GeometricFunctions.h"
#include "TrigonometricFunctions.h"
#include "Constants.h"
#include "ValuePointer.h"
#include "Hashes.h"

namespace Math {
    using Vector2f = Types::Vector<float, 2>;
    using Vector3f = Types::Vector<float, 3>;
    using Vector4f = Types::Vector<float, 4>;
}

#endif