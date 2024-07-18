#ifndef _BXDF_H
#define _BXDF_H

#include "../Material.h"
#include "../Ray.h"
#include "../HitPayload.h"
#include "../math/Math.h"

//! Bidirectional reflectance/scattering/x distribution function class
class BXDF {
public:
    //! Creates BxDF based on passed material
    constexpr BXDF(const Material *material) noexcept :
        m_Material(material) {}

    //! Returns direction of reflected ray and modifies throughput
    Math::Vector3f Sample(Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept;

private:
    Math::Vector3f SampleBRDF(const Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept;
    
    Math::Vector3f SampleBSDF(Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept;

private:
    const Material *m_Material;
};

#endif