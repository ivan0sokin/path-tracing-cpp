#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <vector>
#include <cstdint>

#include "Texture.h"
#include "math/Math.h"

namespace TextureIndex {
    enum {
        Albedo = 0,
        Metallic,
        Specular,
        Roughness,
        Bump
    };
}

//! Physically based material type. Also can be serialized/deserialized
struct Material {
    Texture* textures[5];
    float emissionPower;

    int index;

    // float transparency = 0.f;
    // float refractionIndex = 1.f;

    constexpr Material() noexcept :
        textures{nullptr, nullptr, nullptr, nullptr, nullptr}, emissionPower(0.f), index(-1) {}

    //! Returns emmision of material 
    inline Math::Vector3f GetEmission(const Math::Vector2f &texcoords) const noexcept {
        return textures[TextureIndex::Albedo]->PickValue(texcoords) * emissionPower;
    }
};

#endif