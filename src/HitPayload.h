#ifndef _HIT_PAYLOAD_H
#define _HIT_PAYLOAD_H

#include "math/Math.h"
#include "Material.h"
#include "Ray.h"

//! Holds information about ray-surface interaction
struct HitPayload {
    float t;
    Math::Vector3f normal;
    Math::Vector2f texcoord;
    Ray transformedRay;
    const Material *material;
};

#endif