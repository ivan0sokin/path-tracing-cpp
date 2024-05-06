#ifndef _LIGHT_H
#define _LIGHT_H

#include "hittable/HittableObject.h"
#include "math/Math.h"
#include "Ray.h"
#include "HitPayload.h"

class Light {
public:
    constexpr Light(const HittableObjectPtr object, const Math::Vector3f &emission) noexcept :
        m_Object(object), m_Emission(emission) {}

    const HittableObject* GetObject() const noexcept {
        return m_Object;
    }

    Math::Vector3f Sample(const Ray &lightRay, const HitPayload &objectHitPayload, const HitPayload &lightHitPayload, float distance, float distanceSquared) const noexcept {
        constexpr float distanceEpsilon = 0.01f;
        if (Math::Abs(lightHitPayload.t - distance) > distanceEpsilon) {
            return Math::Vector3f(0.f);
        }
        
        float pdf = distanceSquared / (Math::Dot(lightHitPayload.normal, -lightRay.direction) * m_Object->GetArea());
        
        constexpr float pdfEpsilon = 0.01f;
        if (pdf <= pdfEpsilon) {
            return Math::Vector3f(0.f);
        }
        
        Math::Vector3f brdf = objectHitPayload.material->albedo * Math::Constants::InversePi<float> * Math::Dot(objectHitPayload.normal, lightRay.direction) * m_Emission;
        
        return brdf / pdf;
    }

private:
    const HittableObject* m_Object;
    Math::Vector3f m_Emission;
};

#endif