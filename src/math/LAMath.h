#ifndef _LAMATH_H
#define _LAMATH_H

#include "Types.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "MatrixCommon.h"
#include "ElementaryFunctions.h"
#include "GeometricFunctions.h"
#include "TrigonometricFunctions.h"
#include "Constants.h"
#include "ValuePointer.h"
#include "Hashes.h"
#include "Transform.h"

//! Linear algebra and basic geometric, trigonometric math
namespace Math {
    using Vector2f = Types::Vector<float, 2>;
    using Vector3f = Types::Vector<float, 3>;
    using Vector4f = Types::Vector<float, 4>;

    using Matrix3f = Types::Matrix<float, 3, 3>;
    using Matrix4f = Types::Matrix<float, 4, 4>;
}

#endif
