#ifndef _HIT_PAYLOAD_H
#define _HIT_PAYLOAD_H

#include "Primitive.h"

#include "../glm/include/glm/vec3.hpp"

struct HitPayload {
    Primitive primitive;
    int objectIndex;
    float t;
    glm::vec3 point;
    glm::vec3 normal;
};

#endif