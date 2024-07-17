#ifndef _BLAS_H
#define _BLAS_H

#include <vector>

#include "BVH.h"
#include "../Ray.h"

//! Bottom-level acceleration structure. Works as BVH instance, holds information about transformation
class BLAS {
public:
    constexpr ~BLAS() noexcept = default;

    //! Constructs BLAS with given BVH and no transformation
    constexpr BLAS(const BVH *bvh) noexcept :
        m_BVH(bvh),
        m_LocalAABB(bvh->GetBoundingBox()),
        m_Transform(Math::IdentityMatrix<float, 4>()),
        m_InverseTransform(Math::IdentityMatrix<float, 4>()) {}

    //! Sets transformation matrix
    constexpr void SetTransform(const Math::Matrix4f &transform) noexcept {
        m_Transform = transform;
        m_InverseTransform = Math::Inverse(transform);
        m_LocalAABB = ComputeLocalAABB(transform);
    }

    //! Ray-BLAS intersection. Transforms ray into local space and saves it if hit
    inline bool Hit(const Ray &worldRay, float tMin, float tMax, HitPayload &payload) const noexcept {
        if (m_LocalAABB.Intersect(worldRay, tMin, tMax) == Math::Constants::Infinity<float>) {
            return false;
        }

        Ray localRay;
        localRay.origin = Math::TransformPoint(m_InverseTransform, worldRay.origin);
        localRay.direction = Math::TransformVector(m_InverseTransform, worldRay.direction);
        localRay.inverseDirection = 1.f / localRay.direction;

        if (!m_BVH->Hit(localRay, tMin, tMax, payload)) {
            return false;
        }
        
        payload.localRay = localRay;
        payload.transform = m_Transform;

        return true;
    }

    //! Returns AABB in local space
    constexpr AABB GetLocalBoundingBox() const noexcept {
        return m_LocalAABB;
    }

    //! Returns its BVH
    constexpr const BVH* GetBVH() const noexcept {
        return m_BVH;
    }

private:
    constexpr AABB ComputeLocalAABB(const Math::Matrix4f transform) const noexcept {
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

        return AABB(min, max);
    }

private:
    const BVH *m_BVH;
    AABB m_LocalAABB;
    Math::Matrix4f m_Transform;
    Math::Matrix4f m_InverseTransform;
};

#endif