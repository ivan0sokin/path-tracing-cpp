#ifndef _RAY_H
#define _RAY_H

#include "math/Math.h"

//! Ray representation, P(t) = origin + direction * t
struct Ray {
    Math::Vector3f origin;
    //! Direction must always be unit
    Math::Vector3f direction;
};

#endif