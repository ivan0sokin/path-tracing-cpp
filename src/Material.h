#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "math/Math.h"

struct Material {
    Math::Vector3f albedo{0.f, 0.f, 0.f};
    float metallic = 0.f;
    float specular = 0.f;
    float roughness = 1.f;

    float emissionPower = 0.f;

    constexpr Math::Vector3f GetEmission() const noexcept {
        return albedo * emissionPower;
    }
};

#endif