#ifndef _HIT_PAYLOAD_H
#define _HIT_PAYLOAD_H

#include "Primitive.h"
#include "Material.h"

#include "../glm/include/glm/vec3.hpp"

struct HitPayload {
    float t;
    glm::vec3 point;
    glm::vec3 normal;
    Material material;
};

#endif