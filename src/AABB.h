#ifndef _AABB_H
#define _AABB_H

#include "math/Math.h"

struct AABB {
    Math::Vector3f min, max;

    constexpr AABB() noexcept {
        *this = AABB::Empty;
    }

    constexpr AABB(const Math::Vector3f &a, const Math::Vector3f &b) noexcept :
        min(Math::Min(a, b)), max(Math::Max(a, b)) {}

    constexpr AABB(const AABB &other) noexcept = default;

    constexpr AABB(const AABB &a, const AABB &b) noexcept :
        min(Math::Min(a.min, b.min)), max(Math::Max(a.max, b.max)) {}


    constexpr bool IntersectsRay(const Ray &ray, float t_min, float t_max) const noexcept {
        // Math::Vector3f center = (min + max) * 0.5f;
        // Math::Vector3f extent = max - min;

        // Math::Vector3f localOrigin = ray.origin - center;
        // Math::Vector3f direction = ray.direction;
        // if ((Math::Abs(localOrigin.x) > extent.x && localOrigin.x * direction.x >= 0.f) ||
        //     (Math::Abs(localOrigin.y) > extent.y && localOrigin.y * direction.y >= 0.f) ||
        //     (Math::Abs(localOrigin.z) > extent.z && localOrigin.z * direction.z >= 0.f)) {
        //         return false;
        // }

        // Math::Vector3f WxD = Math::Cross(direction, localOrigin);
        // Math::Vector3f absWdU(Math::Abs(direction.x), Math::Abs(direction.y), Math::Abs(direction.z));
        // Math::Vector3f lines(Math::Dot(Math::Vector2f(extent.y, extent.z), Math::Vector2f(absWdU.z, absWdU.y)),
        //                      Math::Dot(Math::Vector2f(extent.x, extent.z), Math::Vector2f(absWdU.z, absWdU.x)),
        //                      Math::Dot(Math::Vector2f(extent.x, extent.y), Math::Vector2f(absWdU.y, absWdU.x)));
        // if (WxD.x > lines.x ||
        //     WxD.y > lines.y ||
        //     WxD.z > lines.z) {
        //         return false;
        // }

        // return true;
        
        // float t_min = 0.001f;
        // float t_max = Math::Cons tants::Max<float>; 

        // auto one_over_direction = 1.0 / ray.direction.x;
        // auto origin = ray.origin.x;

        // auto t0 = (min.x - origin) * one_over_direction;
        // auto t1 = (max.x - origin) * one_over_direction;

        // if (one_over_direction < 0.f) {
        //     std::swap(t0, t1);
        // }

        // if (t0 > t_min) {
        //     t_min = t0;
        // }

        // if (t1 < t_max) {
        //     t_max = t1;
        // }

        // if (t_max <= t_min) {
        //     return false;
        // }

        // one_over_direction = 1.0 / ray.direction.y;
        // origin = ray.origin.y;

        // t0 = (min.y - origin) * one_over_direction;
        // t1 = (max.y - origin) * one_over_direction;

        // if (one_over_direction < 0.f) {
        //     std::swap(t0, t1);
        // }

        // if (t0 > t_min) {
        //     t_min = t0;
        // }

        // if (t1 < t_max) {
        //     t_max = t1;
        // }

        // if (t_max <= t_min) {
        //     return false;
        // }

        // one_over_direction = 1.0 / ray.direction.z;
        // origin = ray.origin.z;

        // t0 = (min.z - origin) * one_over_direction;
        // t1 = (max.z - origin) * one_over_direction;

        // if (one_over_direction < 0.f) {
        //     std::swap(t0, t1);
        // }

        // if (t0 > t_min) {
        //     t_min = t0;
        // }

        // if (t1 < t_max) {
        //     t_max = t1;
        // }

        // if (t_max <= t_min) {
        //     return false;
        // }

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

    static const AABB Empty;
    static const AABB All;
};

constexpr AABB AABB::Empty = {
    Math::Vector3f(Math::Constants::Infinity<float>),
    Math::Vector3f(-Math::Constants::Infinity<float>)
};

constexpr AABB AABB::All = {
    Math::Vector3f(-Math::Constants::Infinity<float>),
    Math::Vector3f(+Math::Constants::Infinity<float>)
};

#endif