#ifndef _HIT_PAYLOAD_H
#define _HIT_PAYLOAD_H

#include "math/Math.h"
#include "Material.h"

//! Holds information about ray-surface interaction
struct HitPayload {
    float t;
    Math::Vector3f normal;
    Math::Vector2f texcoord;
    const Material *material;
};

#endif