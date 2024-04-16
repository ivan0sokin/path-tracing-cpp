#ifndef _AABB_H
#define _AABB_H

#include "math/Math.h"

struct AABB {
    Math::Vector3f min, max;
};

#endif