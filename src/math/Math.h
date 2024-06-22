#ifndef _MATH_H
#define _MATH_H

#include "Types.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "MatrixCommon.h"
#include "ElementaryFunctions.h"
#include "GeometricFunctions.h"
#include "TrigonometricFunctions.h"
#include "Constants.h"
#include "ValuePointer.h"
#include "Hashes.h"
#include "Transformation.h"

namespace Math {
    using Vector2f = Types::Vector<float, 2>;
    using Vector3f = Types::Vector<float, 3>;
    using Vector4f = Types::Vector<float, 4>;

    using Matrix4x4f = Types::Matrix<float, 4, 4>;
}

#endif