#ifndef _IHITTABLE_H
#define _IHITTABLE_H

#include "../HitPayload.h"
#include "../math/Math.h"
#include "../Ray.h"
#include "../AABB.h"

//! Abstraction for hittable object
class IHittable {
public:
    constexpr IHittable() = default;
    constexpr IHittable(const IHittable&) = default;
    constexpr IHittable(IHittable&&) = default;
    constexpr IHittable& operator=(const IHittable&) = default;

    constexpr virtual ~IHittable() = default;

    virtual bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept = 0;

    virtual Math::Vector3f GetCentroid() const noexcept = 0;

    virtual AABB GetBoundingBox() const noexcept = 0;

    virtual Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept = 0;

    virtual float GetSurfaceArea() const noexcept = 0;
};

using HittableObjectPtr = IHittable*;

#endif