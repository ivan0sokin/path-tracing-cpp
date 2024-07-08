#ifndef _BLAS_H
#define _BLAS_H

#include <vector>

#include "BVH.h"
#include "../Ray.h"

class BLAS {
public:
    constexpr ~BLAS() noexcept = default;

    inline BLAS(const BVH *bvh) noexcept :
        m_BVH(bvh), m_TransformedAABB(bvh->GetBoundingBox()), m_InverseTransform(Math::IdentityMatrix<float, 4>()) {}

    inline void SetTransform(const Math::Matrix4f &transform) noexcept {
        m_InverseTransform = Math::Inverse(transform);

        AABB aabb = m_BVH->GetBoundingBox();
        Math::Vector3f min, max;
        for (int i = 0; i < 8; ++i) {
            Math::Vector3f corner;
            corner.x = i & 1 ? aabb.max.x : aabb.min.x;
            corner.y = i & 2 ? aabb.max.y : aabb.min.y;
            corner.z = i & 4 ? aabb.max.z : aabb.min.z;

            Math::Vector3f point = Math::TransformPoint(transform, corner);
            min = Math::Min(min, point);
            max = Math::Max(max, point);
        }

        m_TransformedAABB = AABB(min, max);
    }

    inline bool Hit(const Ray &worldRay, float tMin, float tMax, HitPayload &payload) const noexcept {
        if (m_TransformedAABB.Intersect(worldRay, tMin, tMax) == Math::Constants::Infinity<float>) {
            return false;
        }

        Ray ray;
        ray.origin = Math::TransformPoint(m_InverseTransform, worldRay.origin);
        ray.direction = Math::Normalize(Math::TransformVector(m_InverseTransform, worldRay.direction));
        ray.oneOverDirection = 1.f / ray.direction;

        if (!m_BVH->Hit(ray, tMin, tMax, payload)) {
            return false;
        }
        
        payload.transformedRay = ray;
        return true;
    }

    constexpr AABB GetTransformedBoundingBox() const noexcept {
        return m_TransformedAABB;
    }

    constexpr const BVH* GetBVH() const noexcept {
        return m_BVH;
    }

private:
    const BVH *m_BVH;
    AABB m_TransformedAABB;
    Math::Matrix4f m_InverseTransform;
};

#endif