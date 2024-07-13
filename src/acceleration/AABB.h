#ifndef _AABB_H
#define _AABB_H

#include "../math/Math.h"
#include "../Ray.h"

//! Axis aligned bounding box, structure used to make faster ray-model intersections
struct AABB {
    Math::Vector3f min, max;

    //! Constructs default value AABB (should not be used)
    constexpr AABB() noexcept = default;

    //! Creates AABB from two given points
    constexpr AABB(const Math::Vector3f &a, const Math::Vector3f &b) noexcept :
        min(Math::Min(a, b)), max(Math::Max(a, b)) {}

    //! Copy constructor
    constexpr AABB(const AABB &other) noexcept = default;

    //! Unites two AABBs into one
    constexpr AABB(const AABB &a, const AABB &b) noexcept :
        min(Math::Min(a.min, b.min)), max(Math::Max(a.max, b.max)) {}

    //! Returns surface area
    constexpr float GetSurfaceArea() const noexcept {
        float a = max.x - min.x;
        float b = max.y - min.y;
        float c = max.z - min.z;
        return 2.f * (a * b + b * c + a * c);
    }

    //! Returns true if ray intersects AABB on give interval
    constexpr float Intersect(const Ray &ray, float tMin, float tMax) const noexcept {
        auto inverseDirection = ray.inverseDirection;
        auto origin = ray.origin;

        auto t0 = (min - origin) * inverseDirection;
        auto t1 = (max - origin) * inverseDirection;

        if (inverseDirection.x < 0.f) {
            std::swap(t0.x, t1.x);
        }
        
        if (inverseDirection.y < 0.f) {
            std::swap(t0.y, t1.y);
        }

        if (inverseDirection.z < 0.f) {
            std::swap(t0.z, t1.z);
        }

        tMin = Math::Max(tMin, Math::MaxComponent(t0));
        tMax = Math::Min(tMax, Math::MinComponent(t1));

        if (tMax <= tMin) {
            return Math::Constants::Infinity<float>;
        }

        return tMin;
    }

    //! Returns empty AABB i.e. where min = +inf, max = -inf
    constexpr static AABB Empty() noexcept {
        AABB aabb;
        aabb.min = Math::Vector3f(+Math::Constants::Infinity<float>);
        aabb.max = Math::Vector3f(-Math::Constants::Infinity<float>);
        return aabb;
    }
};

#endif