#ifndef _ACCELERATION_STRUCTURE_H
#define _ACCELERATION_STRUCTURE_H

#include "hittable/HittableObject.h"
#include "math/Math.h"
#include "AABB.h"
#include "Ray.h"
#include "HitPayload.h"
#include "BVHNode.h"

#include <vector>
#include <functional>

class AccelerationStructure {
public:
    constexpr AccelerationStructure() = default;

    AccelerationStructure(std::vector<const HittableObject*> &objects) noexcept;

    void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept;

private:
    BVHNode* Build(std::vector<HittableObject*> &objects, int low, int high) noexcept;

private:
    BVHNode *m_Root = nullptr;
};

#endif