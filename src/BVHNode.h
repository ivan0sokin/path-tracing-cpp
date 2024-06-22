#ifndef _BVHNODE_H
#define _BVHNODE_H

#include "Ray.h"
#include "AABB.h"
#include "HitPayload.h"
#include "hittable/HittableObject.h"

#include <span>
#include <vector>
#include <functional>
#include <stack>
#include <cstdio>
//! Bounding volume hierarchy node
struct BVHNode {
    AABB aabb = AABB::Empty();
    BVHNode *left = nullptr, *right = nullptr;
    const IHittable *object = nullptr;

    constexpr BVHNode() noexcept = default;

    constexpr BVHNode(BVHNode *left, BVHNode *right) noexcept :
        left(left), right(right), aabb(left->aabb, right->aabb) {}

    constexpr BVHNode(const IHittable *object) noexcept :
        object(object), aabb(object->GetBoundingBox()) {}

    //! Returns true if this node contains pointer to primitive
    constexpr bool IsTerminating() const noexcept {
        return object != nullptr;
    }

    //! Frees allocated memory of tree with root ```node```
    constexpr static void FreeMemory(BVHNode *node) noexcept {
        if (node == nullptr) {
            return;
        }

        FreeMemory(node->left);
        FreeMemory(node->right);

        delete node;
    }

    // Math::Matrix4x4f invTrf = Math::Inverse(Math::TranslationMatrix(Math::Vector3f(0.2f, 0.f, 0.f)));
    Math::Matrix4x4f invTrf = Math::Inverse(Math::RotationMatrix(Math::Vector3f(0.f, Math::ToRadians(90.f), 0.f)));

    //! Saves ray-BVH intersection info into ```payload```
    inline bool Hit(const Ray &worldRay, float tMin, float tMax, HitPayload &payload) noexcept {
        // Ray ray = r;
        Ray ray;
        ray.origin = Math::TransformPoint(invTrf, worldRay.origin);
        ray.direction = Math::TransformVector(invTrf, worldRay.direction);
        ray.oneOverDirection = 1.f / ray.direction;

        if (this->aabb.Intersect(ray, tMin, tMax) == Math::Constants::Infinity<float>) {
            return false;
        }

        const int TREE_DEPTH = 1024;

        BVHNode *node = this;
        BVHNode* nodes[TREE_DEPTH];
        int stackPointer = 1;

        bool anyHit = false;
        while (stackPointer > 0) {
            if (node->IsTerminating()) {
                anyHit |= node->object->Hit(ray, tMin, tMax, payload);
                tMax = Math::Min(tMax, payload.t);
                
                node = nodes[--stackPointer];
                continue;
            }

            BVHNode *closestChild = node->left;
            BVHNode *furtherChild = node->right;

            float closestT = closestChild->aabb.Intersect(ray, tMin, tMax);
            float furtherT = furtherChild->aabb.Intersect(ray, tMin, tMax);

            if (closestT > furtherT) {
                std::swap(closestT, furtherT);
                std::swap(closestChild, furtherChild);
            }

            if (closestT == Math::Constants::Infinity<float>) {
                node = nodes[--stackPointer];
                continue;
            }

            node = closestChild;

            if (furtherT != Math::Constants::Infinity<float>) {
                nodes[stackPointer++] = furtherChild;
            }
        }

        return anyHit;
    }

    //! Makes BVH using Sweep SAH optimization
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
    
    //! Makes BVH using naive implementation sorting by lengths of axes
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
};

#endif