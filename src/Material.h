#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "../glm/include/glm/vec3.hpp"

struct Material {
    glm::vec3 albedo{0.f, 0.f, 0.f};
    glm::vec3 emissionColor{0.f, 0.f, 0.f};
    float emissionPower = 0.f;
    float reflectance = 0.f;
    float fuzziness = 0.f;

    constexpr glm::vec3 GetEmission() const noexcept {
        return emissionColor * emissionPower;
    }
};

#endif