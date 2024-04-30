#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "math/Math.h"

struct Material {
    Math::Vector3f albedo;
    float metallic;
    float specular;
    float roughness;
    float emissionPower;

    int index;

    constexpr Math::Vector3f GetEmission() const noexcept {
        return albedo * emissionPower;
    }
};

#endif