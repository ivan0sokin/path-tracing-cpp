#ifndef _IHITTABLE_H
#define _IHITTABLE_H

#include "../HitPayload.h"
#include "../math/Math.h"
#include "../Ray.h"
#include "../acceleration/AABB.h"

//! Abstraction for hittable object
class IHittable {
public:
    constexpr IHittable() = default;
    constexpr IHittable(const IHittable&) = default;
    constexpr virtual ~IHittable() = default;

    constexpr IHittable(IHittable&&) = default;
    constexpr IHittable& operator=(const IHittable&) = default;

    //! Performs Ray-IHittable intersection. Should return true if hit
    virtual bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept = 0;

    //! Returns centroid of geometric shape. Centroid is mass center
    virtual Math::Vector3f GetCentroid() const noexcept = 0;

    //! Returns AABB of the shape. It must contain entire shape
    virtual AABB GetBoundingBox() const noexcept = 0;

    //! Returns point on surface. Should uniformly sample surface
    virtual Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept = 0;

    //! Returns surface area of shape
    virtual float GetSurfaceArea() const noexcept = 0;
};

#endif