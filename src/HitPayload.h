#ifndef _HIT_PAYLOAD_H
#define _HIT_PAYLOAD_H

#include "Primitive.h"
#include "Material.h"

#include "math/Math.h"

struct HitPayload {
    float t;
    Math::Vector3f point;
    Math::Vector3f normal;
    Material material;
};

#endif