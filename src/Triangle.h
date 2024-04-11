#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "math/Math.h"

struct Triangle {
    Math::Vector3f vertices[3];
    Math::Vector3f edges[2];
    Math::Vector3f normal;
    int materialIndex;

    constexpr Triangle(const Math::Vector3f &a, const Math::Vector3f &b, const Math::Vector3f &c, int materialIndex) noexcept :
        vertices{a, b, c}, edges{b - a, c - a}, normal(Math::Cross(edges[0], edges[1])), materialIndex(materialIndex) {}
};

#endif