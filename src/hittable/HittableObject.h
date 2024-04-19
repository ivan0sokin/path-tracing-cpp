#ifndef _HITTABLE_OBJECT_H
#define _HITTABLE_OBJECT_H

#include "../math/Math.h"
#include "../Ray.h"

#include <utility>

class HittableObject {
public:
    constexpr HittableObject() = default;
    constexpr HittableObject(const HittableObject&) = default;
    constexpr HittableObject(HittableObject&&) = default;
    constexpr HittableObject& operator=(const HittableObject&) = default;

    constexpr virtual ~HittableObject() = default;

    virtual std::pair<float, Math::Vector3f> TryHit(const Ray &ray) const noexcept = 0;

    virtual int GetMaterialIndex() const noexcept = 0;
};

#endif