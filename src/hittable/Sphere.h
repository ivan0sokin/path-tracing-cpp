#ifndef _SPHERE_H
#define _SPHERE_H

#include "../Material.h"
#include "HittableObject.h"
#include "../math/Math.h"
#include "../AABB.h"
#include "../HitPayload.h"

namespace Shapes {
    struct Sphere : public HittableObject {
        Math::Vector3f center;
        float radius, radiusSquared, inverseRadius;
        int materialIndex;

        constexpr Sphere(const Math::Vector3f &center, float radius, int materialIndex) noexcept :
            center(center), radius(radius), radiusSquared(radius * radius), inverseRadius(1.f / radius), materialIndex(materialIndex) {}

        inline void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override {
            Math::Vector3f centerToOrigin = ray.origin - center;
        
            float a = Math::Dot(ray.direction, ray.direction);
            float k = Math::Dot(centerToOrigin, ray.direction);
            float c = Math::Dot(centerToOrigin, centerToOrigin) - radiusSquared;
            float discriminant = k * k - a * c;
            
            if (discriminant < 0.0) {
                return;
            }

            float t0 = (-k - Math::Sqrt(discriminant)) / a;
            float t1 = (-k + Math::Sqrt(discriminant)) / a;

            float t = t0;
            if (t < tMin || tMax < t) {
                t = t1;
                if (t < tMin || tMax < t) {
                    return;
                }
            }
            
            payload.t = t;
            payload.normal = (ray.origin + ray.direction * t - center) * inverseRadius;
            payload.materialIndex = materialIndex;
        }

        inline int GetMaterialIndex() const noexcept override {
            return materialIndex;
        }

        inline AABB GetBoundingBox() const noexcept override {
            return AABB(center - radius, center + radius);
        }
    };
}

#endif