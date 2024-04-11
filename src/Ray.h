#ifndef _RAY_H
#define _RAY_H

#include "../glm/include/glm/vec3.hpp"

struct Ray {
    glm::vec3 origin, direction;
};

#endif