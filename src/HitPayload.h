#ifndef _HIT_PAYLOAD_H
#define _HIT_PAYLOAD_H

#include "Material.h"

#include "math/Math.h"

struct HitPayload {
    float t;
    Math::Vector3f point;
    Math::Vector3f normal;
    Math::Vector3f orientedNormal;
    int materialIndex;
};

#endif