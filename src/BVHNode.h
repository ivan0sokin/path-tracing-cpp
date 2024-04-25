#ifndef _BVHNODE_H
#define _BVHNODE_H

#include "Ray.h"
#include "AABB.h"
#include "HitPayload.h"
#include "hittable/HittableObject.h"

#include <vector>
#include <functional>

struct BVHNode {
    AABB aabb = AABB::Empty();
    BVHNode *left = nullptr, *right = nullptr;
    const HittableObject *object = nullptr;

    constexpr BVHNode() noexcept = default;

    constexpr BVHNode(BVHNode *left, BVHNode *right) noexcept :
        left(left), right(right), aabb(left->aabb, right->aabb) {}

    constexpr BVHNode(const HittableObject *object) noexcept :
        object(object), aabb(object->GetBoundingBox()) {}

    constexpr bool IsTerminating() const noexcept {
        return object != nullptr;
    }

    inline void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) noexcept {
        if (!aabb.IntersectsRay(ray, tMin, tMax)) {
            return;
        }

        if (IsTerminating()) {
            object->Hit(ray, tMin, tMax, payload);
            return;
        }

        left->Hit(ray, tMin, tMax, payload);
        right->Hit(ray, tMin, Math::Min(tMax, payload.t), payload);
    }
    
    inline static BVHNode* MakeHierarchy(std::vector<const HittableObject*> &objects, int low, int high) noexcept {
        AABB aabb = AABB::Empty();
        for (int i = low; i < high; ++i) {
            aabb = AABB(aabb, objects[i]->GetBoundingBox());
        }

        int longestAxisIndex = 0;
        float longestAxisLength = aabb.max.x - aabb.min.x;
        
        if (aabb.max.y - aabb.min.y > longestAxisLength) {
            longestAxisIndex = 1;
            longestAxisLength = aabb.max.y - aabb.min.y;
        }

        if (aabb.max.z - aabb.min.z > longestAxisLength) {
            longestAxisIndex = 2;
            longestAxisLength = aabb.max.z - aabb.min.z;
        }
        
        auto comparator = c_Comparators[longestAxisIndex];

        if (low + 1 == high) {
            return new BVHNode(objects[low]);
        }

        std::sort(objects.begin() + low, objects.begin() + high, comparator);

        int mid = (low + high) / 2;
        BVHNode *left = MakeHierarchy(objects, low, mid);
        BVHNode *right = MakeHierarchy(objects, mid, high);

        return new BVHNode(left, right);
    }

    inline static const std::function<bool(const HittableObject*, const HittableObject*)> c_Comparators[3] = {
        [](const HittableObject *a, const HittableObject *b){
            return a->GetBoundingBox().min.x < b->GetBoundingBox().min.x;
        },
        [](const HittableObject *a, const HittableObject *b){
            return a->GetBoundingBox().min.y < b->GetBoundingBox().min.y;
        },
        [](const HittableObject *a, const HittableObject *b){
            return a->GetBoundingBox().min.z < b->GetBoundingBox().min.z;
        }
    };
};

#endif