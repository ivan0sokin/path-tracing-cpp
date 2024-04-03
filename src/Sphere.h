#ifndef _SPHERE_H
#define _SPHERE_H

#include "../glm/include/glm/vec3.hpp"

struct Sphere {
    glm::vec3 center;
    float radius, radiusSquared, inverseRadius;
    int materialIndex;

    constexpr Sphere(const glm::vec3 &center, float radius, int materialIndex) noexcept :
        center(center), radius(radius), radiusSquared(radius * radius), inverseRadius(1.f / radius), materialIndex(materialIndex) {}
};

#endif