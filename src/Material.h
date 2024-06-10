#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <vector>
#include <cstdint>

#include "Texture.h"
#include "math/Math.h"

//! Physically based material type. Also can be serialized/deserialized
struct Material {
    Texture albedo;
    Texture metallic;
    Texture specular;
    Texture roughness;
    Texture bump;
    float emissionPower;

    int index;

    //! Returns emmision of material 
    inline Math::Vector3f GetEmission(const Math::Vector2f &texcoords) const noexcept {
        return albedo.PickValue(texcoords) * emissionPower;
    }
};

#endif