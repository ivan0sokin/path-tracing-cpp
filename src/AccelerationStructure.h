#ifndef _ACCELERATION_STRUCTURE_H
#define _ACCELERATION_STRUCTURE_H

#include "hittable/HittableObject.h"
#include "math/Math.h"
#include "AABB.h"
#include "Ray.h"
#include "HitPayload.h"
#include "BVHNode.h"

#include <span>

//! Class which holds all information about accelerating scene
class AccelerationStructure {
public:
    //! Creates Acceleration structure that cannot be hit
    AccelerationStructure() noexcept;

    //! Creates Acceleration structure from list of objects
    AccelerationStructure(std::span<HittableObjectPtr> objects) noexcept;
    
    //! Deallocates BVH
    ~AccelerationStructure() noexcept;

    //! Replaces old BVH with new one constructed with ```objects```
    void Update(std::span<HittableObjectPtr> objects) noexcept;

    //! Saves info about hit into ```payload```
    void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept;

private:
    BVHNode *m_Root = nullptr;
};

#endif