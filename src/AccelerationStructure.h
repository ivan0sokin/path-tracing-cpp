#ifndef _ACCELERATION_STRUCTURE_H
#define _ACCELERATION_STRUCTURE_H

#include "hittable/HittableObject.h"
#include "math/Math.h"
#include "AABB.h"
#include "Ray.h"
#include "HitPayload.h"

#include <vector>
#include <functional>

class AccelerationStructure {
public:
    AccelerationStructure(std::vector<HittableObject*> &objects) noexcept;

    void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept;

private:
    struct BVHNode {
        AABB aabb = AABB::Empty;
        const HittableObject *object = nullptr;

        constexpr BVHNode() noexcept = default;

        constexpr BVHNode(const BVHNode &left, const BVHNode &right) noexcept :
            aabb(left.aabb, right.aabb) {}

        constexpr BVHNode(const HittableObject *object) noexcept :
            object(object), aabb(object->GetBoundingBox()) {}

        constexpr bool IsTerminating() const noexcept {
            return object != nullptr;
        }
    };

    void Build(std::vector<HittableObject*> &objects, int index, int low, int high) noexcept;
    
    void HitBVHNode(int index, const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept;

private:
    inline static std::function<bool(HittableObject*, HittableObject*)> m_Comparators[3] = {
        [](HittableObject* a, HittableObject* b){
            return a->GetBoundingBox().min.x < b->GetBoundingBox().min.x;
        },
        [](HittableObject* a, HittableObject* b){
            return a->GetBoundingBox().min.y < b->GetBoundingBox().min.y;
        },
        [](HittableObject* a, HittableObject* b){
            return a->GetBoundingBox().min.z < b->GetBoundingBox().min.z;
        }
    };

private:
    BVHNode *m_Nodes = nullptr;
};

#endif