#ifndef _HIT_PAYLOAD_H
#define _HIT_PAYLOAD_H

#include "math/Math.h"
#include "Material.h"

struct HitPayload {
    float t;
    Math::Vector3f normal;
    const Material *material;
};

#endif