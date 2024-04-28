#ifndef _BVHNODE_H
#define _BVHNODE_H

#include "Ray.h"
#include "AABB.h"
#include "HitPayload.h"
#include "hittable/HittableObject.h"

#include <span>
#include <vector>
#include <functional>
#include <cstdio>

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

    inline static BVHNode* MakeHierarchySAH(std::span<HittableObjectPtr> objects, int low, int high) noexcept {
        if (low + 1 == high) {
            return new BVHNode(objects[low]);
        }
        
        int n = high - low;
        std::vector<AABB> pref(n + 1);
        std::vector<AABB> suff(n + 1);

        float minValue = Math::Constants::Infinity<float>;
        int mid = -1;
        int dimension = -1;

        for (int d = 0; d < 3; ++d) {
            std::sort(objects.begin() + low, objects.begin() + high, c_ComparatorsSAH[d]);

            pref[0] = AABB::Empty();
            for (int i = 0; i < n; ++i) {
                pref[i + 1] = AABB(pref[i], objects[i + low]->GetBoundingBox());
            }

            suff[n] = AABB::Empty();
            for (int i = n - 1; i >= 0; --i) {
                suff[i] = AABB(objects[i + low]->GetBoundingBox(), suff[i + 1]);
            }

            float minValueAlongAxis = Math::Constants::Infinity<float>;
            int index = -1;
            for (int i = 0; i < n; ++i) {
                float value = pref[i + 1].GetSurfaceArea() * (float)(i + 1) + suff[i + 1].GetSurfaceArea() * (float)(n - i - 1);
                if (value < minValueAlongAxis) {
                    minValueAlongAxis = value;
                    index = i + low;
                }
            }

            if (minValueAlongAxis < minValue) {
                minValue = minValueAlongAxis;
                mid = index + 1;
                dimension = d;
            }
        }

        std::sort(objects.begin() + low, objects.begin() + high, c_ComparatorsSAH[dimension]);
        
        BVHNode *left = MakeHierarchySAH(objects, low, mid);
        BVHNode *right = MakeHierarchySAH(objects, mid, high);

        return new BVHNode(left, right);
    }

    inline static const std::function<bool(HittableObjectPtr, HittableObjectPtr)> c_ComparatorsSAH[3] = {
        [](HittableObjectPtr a, HittableObjectPtr b){
            return a->GetCentroid().x < b->GetCentroid().x;
        },
        [](HittableObjectPtr a, HittableObjectPtr b){
            return a->GetCentroid().y < b->GetCentroid().y;
        },
        [](HittableObjectPtr a, HittableObjectPtr b){
            return a->GetCentroid().z < b->GetCentroid().z;
        }
    };
    
    inline static BVHNode* MakeHierarchyNaive(std::span<HittableObjectPtr> objects, int low, int high) noexcept {
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
        
        auto comparator = c_ComparatorsNaive[longestAxisIndex];

        if (low + 1 == high) {
            return new BVHNode(objects[low]);
        }

        std::sort(objects.begin() + low, objects.begin() + high, comparator);
        
        int mid = (low + high) / 2;
        BVHNode *left = MakeHierarchyNaive(objects, low, mid);
        BVHNode *right = MakeHierarchyNaive(objects, mid, high);

        return new BVHNode(left, right);
    }

    inline static const std::function<bool(HittableObjectPtr, HittableObjectPtr)> c_ComparatorsNaive[3] = {
        [](HittableObjectPtr a, HittableObjectPtr b){
            return a->GetBoundingBox().min.x < b->GetBoundingBox().min.x;
        },
        [](HittableObjectPtr a, HittableObjectPtr b){
            return a->GetBoundingBox().min.y < b->GetBoundingBox().min.y;
        },
        [](HittableObjectPtr a, HittableObjectPtr b){
            return a->GetBoundingBox().min.z < b->GetBoundingBox().min.z;
        }
    };

    constexpr static void FreeMemory(BVHNode *node) noexcept {
        if (node == nullptr) {
            return;
        }

        FreeMemory(node->left);
        FreeMemory(node->right);

        delete node;
    }
};

#endif