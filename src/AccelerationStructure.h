#ifndef _ACCELERATION_STRUCTURE_H
#define _ACCELERATION_STRUCTURE_H

#include "hittable/HittableObject.h"
#include "math/Math.h"
#include "AABB.h"
#include "Ray.h"
#include "HitPayload.h"
#include "BVHNode.h"

#include <span>

class AccelerationStructure {
public:
    AccelerationStructure() noexcept;

    AccelerationStructure(std::span<HittableObjectPtr> objects) noexcept;
    
    ~AccelerationStructure() noexcept;

    void Update(std::span<HittableObjectPtr> objects) noexcept;

    void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept;

private:
    BVHNode *m_Root = nullptr;
};

#endif