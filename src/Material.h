#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "math/Math.h"

//! Strictly defined type that can be serialized/deserialized
struct Material {
    Math::Vector3f albedo;
    float metallic;
    float specular;
    float roughness;
    float emissionPower;

    int index;

    //! Returns emmision of material 
    constexpr Math::Vector3f GetEmission() const noexcept {
        return albedo * emissionPower;
    }
};

#endif