#include "AccelerationStructure.h"
#include "Utilities.hpp"

AccelerationStructure::AccelerationStructure(std::vector<HittableObject*> &objects) noexcept {
    m_Nodes = new BVHNode[objects.size() * 4];
    Build(objects, 0, 0, (int)objects.size());
}

void AccelerationStructure::Build(std::vector<HittableObject*> &objects, int index, int low, int high) noexcept {
    AABB aabb = AABB::Empty;
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
    
    auto comparator = m_Comparators[longestAxisIndex];

    if (low + 1 == high) {
        m_Nodes[index] = BVHNode(objects[low]);
        return;
    }

    std::sort(objects.begin() + low, objects.begin() + high, comparator);

    int mid = (low + high) / 2;
    Build(objects, 2 * index + 1, low, mid);
    Build(objects, 2 * index + 2, mid, high);

    m_Nodes[index] = BVHNode(m_Nodes[2 * index + 1], m_Nodes[2 * index + 2]);
}

void AccelerationStructure::Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept {
    HitBVHNode(0, ray, tMin, tMax, payload);
}

void AccelerationStructure::HitBVHNode(int index, const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept {
    const BVHNode &node = m_Nodes[index];
    
    if (!node.aabb.IntersectsRay(ray, tMin, tMax)) {
        return;
    }

    if (node.IsTerminating()) {
        node.object->Hit(ray, tMin, tMax, payload);
        return;
    }

    HitBVHNode(2 * index + 1, ray, tMin, tMax, payload);
    HitBVHNode(2 * index + 2, ray, tMin, Math::Min(tMax, payload.t), payload);
}