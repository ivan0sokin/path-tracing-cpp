#ifndef _BSDF_H
#define _BSDF_H

#include "../Material.h"
#include "../Ray.h"
#include "../HitPayload.h"
#include "../math/Math.h"

//! Bidirectional scattering distribution function class
class BSDF {
public:
    //! Creates BSDF based on passed material
    constexpr BSDF(const Material *material) noexcept :
        m_Material(material) {}

    //! Returns direction of scattered ray and modifies throughput
    Math::Vector3f Sample(const Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept;

private:
    Math::Vector3f SampleBRDF(const Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept;
    
    Math::Vector3f SampleBTDF(Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept;

private:
    const Material *m_Material;
};

#endif