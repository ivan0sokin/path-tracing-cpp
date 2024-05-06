#ifndef _HITTABLE_OBJECT_H
#define _HITTABLE_OBJECT_H

#include "../HitPayload.h"
#include "../math/Math.h"
#include "../Ray.h"
#include "../AABB.h"

class HittableObject {
public:
    constexpr HittableObject() = default;
    constexpr HittableObject(const HittableObject&) = default;
    constexpr HittableObject(HittableObject&&) = default;
    constexpr HittableObject& operator=(const HittableObject&) = default;

    constexpr virtual ~HittableObject() = default;

    virtual bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept = 0;

    virtual Math::Vector3f GetCentroid() const noexcept = 0;

    virtual AABB GetBoundingBox() const noexcept = 0;

    virtual Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept = 0;

    virtual float GetArea() const noexcept = 0;
};

using HittableObjectPtr = HittableObject*;

#endif