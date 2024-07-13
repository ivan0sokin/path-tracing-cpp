#ifndef _LIGHT_H
#define _LIGHT_H

#include "hittable/IHittable.h"
#include "math/Math.h"
#include "Ray.h"
#include "HitPayload.h"

//! Class that samples lights directly
class Light {
public:
    constexpr Light(const IHittable *object) noexcept :
        m_Object(object) {}

    //! Return pointer to ```object``` that light holds
    const IHittable* GetObject() const noexcept {
        return m_Object;
    }

    //! Returns color sample using direct light sampling method
    Math::Vector3f Sample(const Ray &lightRay, const HitPayload &objectHitPayload, const HitPayload &lightHitPayload, float distance, float distanceSquared) const noexcept {
        constexpr float distanceEpsilon = 0.01f;
        if (Math::Abs(lightHitPayload.t - distance) > distanceEpsilon) {
            return Math::Vector3f(0.f);
        }
        
        float pdf = distanceSquared / (Math::Dot(lightHitPayload.normal, -lightRay.direction) * m_Object->GetSurfaceArea());
        
        constexpr float pdfEpsilon = 0.01f;
        if (pdf <= pdfEpsilon) {
            return Math::Vector3f(0.f);
        }

        Math::Vector3f brdf = objectHitPayload.material->textures[TextureIndex::Albedo]->PickValue(objectHitPayload.texcoord) * lightHitPayload.material->GetEmission(lightHitPayload.texcoord) * Math::Constants::InversePi<float> * Math::Dot(objectHitPayload.normal, lightRay.direction);

        return brdf / pdf;
    }

private:
    const IHittable *m_Object;
};

#endif