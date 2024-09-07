#ifndef _NON_HITTABLE_H
#define _NON_HITTABLE_H

#include "IHittable.h"

//! IHittable that cannot be hit
class NonHittable final : public IHittable {
public:
    constexpr NonHittable() noexcept = default;

    //! Performs no hit. Returns false
    constexpr bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override {
        return false;
    }

    //! Returns zero Vector3f
    constexpr Math::Vector3f GetCentroid() const noexcept override {
        return Math::Vector3f(0.f);
    }

    //! Returns empty AABB
    constexpr AABB GetBoundingBox() const noexcept override {
        return AABB::Empty();
    }

    //! Returns zero Vector3f
    constexpr Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept override {
        return Math::Vector3f(0.f);
    }

    //! Returns zero
    constexpr float GetSurfaceArea() const noexcept override {
        return 0.f;
    }
};

#endif
