#ifndef _SPHERE_H
#define _SPHERE_H

#include "math/Math.h"

struct Sphere {
    Math::Vector3f center;
    float radius, radiusSquared, inverseRadius;
    int materialIndex;

    constexpr Sphere(const Math::Vector3f &center, float radius, int materialIndex) noexcept :
        center(center), radius(radius), radiusSquared(radius * radius), inverseRadius(1.f / radius), materialIndex(materialIndex) {}
};

#endif