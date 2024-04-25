#ifndef _AABB_H
#define _AABB_H

#include "math/Math.h"

struct AABB {
    Math::Vector3f min, max;

    constexpr AABB() noexcept = default;

    constexpr AABB(const Math::Vector3f &a, const Math::Vector3f &b) noexcept :
        min(Math::Min(a, b)), max(Math::Max(a, b)) {}

    constexpr AABB(const AABB &other) noexcept = default;

    constexpr AABB(const AABB &a, const AABB &b) noexcept :
        min(Math::Min(a.min, b.min)), max(Math::Max(a.max, b.max)) {}

    constexpr float GetSurfaceArea() const noexcept {
        float a, b, c;
        a = max.x - min.x;
        b = max.y - min.y;
        c = max.z - min.z;
        return 2.f * a * b + 2.f * b * c + 2.f * a * c;
    }

    constexpr bool IntersectsRay(const Ray &ray, float t_min, float t_max) const noexcept {
        auto one_over_direction = 1.f / ray.direction;
        auto origin = ray.origin;

        auto t0 = (min - origin) * one_over_direction;
        auto t1 = (max - origin) * one_over_direction;

        if (one_over_direction.x < 0.f) {
            std::swap(t0.x, t1.x);
        }
        
        if (one_over_direction.y < 0.f) {
            std::swap(t0.y, t1.y);
        }

        if (one_over_direction.z < 0.f) {
            std::swap(t0.z, t1.z);
        }

        t_min = Math::Max(t_min, Math::Max(Math::Max(t0.x, t0.y), t0.z));
        t_max = Math::Min(t_max, Math::Min(Math::Min(t1.x, t1.y), t1.z));

        if (t_max <= t_min) {
            return false;
        }

        return true;
    }

    constexpr static AABB Empty() noexcept {
        AABB aabb;
        aabb.min = Math::Vector3f(+Math::Constants::Infinity<float>);
        aabb.max = Math::Vector3f(-Math::Constants::Infinity<float>);

        return aabb;
    }
};

#endif