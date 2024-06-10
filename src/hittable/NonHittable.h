#ifndef _NON_HITTABLE_H
#define _NON_HITTABLE_H

#include "HittableObject.h"

//! Hittable object that cannot be hit
class NonHittable : public IHittable {
public:
    constexpr NonHittable() noexcept = default;
    
    constexpr bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override {
        return false;
    }

    constexpr Math::Vector3f GetCentroid() const noexcept override {
        return Math::Vector3f(0.f);
    }

    constexpr AABB GetBoundingBox() const noexcept override {
        return AABB::Empty();
    }

    constexpr Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept override {
        return Math::Vector3f(0.f);
    }

    constexpr float GetArea() const noexcept override {
        return 0.f;
    }
};

#endif