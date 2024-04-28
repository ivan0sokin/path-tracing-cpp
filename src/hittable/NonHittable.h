#ifndef _NON_HITTABLE_H
#define _NON_HITTABLE_H

#include "HittableObject.h"

class NonHittable : public HittableObject {
public:
    constexpr NonHittable() noexcept = default;
    
    constexpr void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override {
        return;
    }

    constexpr Math::Vector3f GetCentroid() const noexcept override {
        return Math::Vector3f(0.f);
    }

    constexpr AABB GetBoundingBox() const noexcept override {
        return AABB::Empty();
    }
};

#endif