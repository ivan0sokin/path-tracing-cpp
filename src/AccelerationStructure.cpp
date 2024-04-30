#include "AccelerationStructure.h"
#include "Utilities.hpp"
#include "hittable/NonHittable.h"

AccelerationStructure::AccelerationStructure() noexcept {
    m_Root = new BVHNode(new NonHittable());
}

AccelerationStructure::AccelerationStructure(std::span<HittableObjectPtr> objects) noexcept {
    Update(objects);
}

AccelerationStructure::~AccelerationStructure() noexcept {
    if (m_Root != nullptr) {
        BVHNode::FreeMemory(m_Root);
    }
}

void AccelerationStructure::Update(std::span<HittableObjectPtr> objects) noexcept {
    if (m_Root != nullptr) {
        BVHNode::FreeMemory(m_Root);
    }

    m_Root = BVHNode::MakeHierarchySAH(objects, 0, static_cast<int>(objects.size()));
}

void AccelerationStructure::Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept {
    m_Root->Hit(ray, tMin, tMax, payload);
}