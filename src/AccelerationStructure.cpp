#include "AccelerationStructure.h"
#include "Utilities.hpp"

AccelerationStructure::AccelerationStructure(std::vector<const HittableObject*> &objects) noexcept {
    // m_Root = Build(objects, 0, (int)objects.size());
    m_Root = BVHNode::MakeHierarchy(objects, 0, (int)objects.size());
}

BVHNode* AccelerationStructure::Build(std::vector<HittableObject*> &objects, int low, int high) noexcept {
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
    
    auto comparator = BVHNode::c_Comparators[longestAxisIndex];

    if (low + 1 == high) {
        // return objects[low];
    }

    std::sort(objects.begin() + low, objects.begin() + high, comparator);

    int mid = (low + high) / 2;
    BVHNode *left = Build(objects, low, mid);
    BVHNode *right = Build(objects, mid, high);

    return new BVHNode(left, right);
}

void AccelerationStructure::Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept {
    m_Root->Hit(ray, tMin, tMax, payload);
}