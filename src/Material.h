#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <vector>
#include <cstdint>

#include "Texture.h"
#include "math/LAMath.h"

//! This namespace is used for indexing textures by name
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

    //! Constructs materual by default
    constexpr Material() noexcept :
        textures{nullptr, nullptr, nullptr, nullptr, nullptr}, emissionPower(0.f), index(-1) {}

    //! Returns emmision of material
    inline Math::Vector3f GetEmission(const Math::Vector2f &texcoords) const noexcept {
        return textures[TextureIndex::Albedo]->PickValue(texcoords) * emissionPower;
    }
};

#endif
