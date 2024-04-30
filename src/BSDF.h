#ifndef _BSDF_H
#define _BSDF_H

#include "Material.h"
#include "Ray.h"
#include "HitPayload.h"
#include "math/Math.h"

class BSDF {
public:
    constexpr BSDF(const Material *material) noexcept :
        m_Material(material) {}

    Math::Vector3f Sample(const Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept;

private:
    const Material *m_Material;
};

#endif