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

    virtual void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept = 0;

    virtual int GetMaterialIndex() const noexcept = 0;

    virtual AABB GetBoundingBox() const noexcept = 0;
};

#endif