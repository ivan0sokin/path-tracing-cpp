#ifndef _SPHERE_H
#define _SPHERE_H

#include "HittableObject.h"
#include "../math/Math.h"

namespace Shapes {
    struct Sphere : public HittableObject {
        Math::Vector3f center;
        float radius, radiusSquared, inverseRadius;
        int materialIndex;

        constexpr Sphere(const Math::Vector3f &center, float radius, int materialIndex) noexcept :
            center(center), radius(radius), radiusSquared(radius * radius), inverseRadius(1.f / radius), materialIndex(materialIndex) {}

        inline std::pair<float, Math::Vector3f> TryHit(const Ray &ray) const noexcept override {
            Math::Vector3f centerToOrigin = ray.origin - center;
        
            float a = Math::Dot(ray.direction, ray.direction);
            float k = Math::Dot(centerToOrigin, ray.direction);
            float c = Math::Dot(centerToOrigin, centerToOrigin) - radiusSquared;
            float discriminant = k * k - a * c;
            
            if (discriminant < 0.0) {
                return {Math::Constants::Max<float>, Math::Vector3f(0.f)};
            }

            float t = (-k - Math::Sqrt(discriminant)) / a;

            return {t, (ray.origin + ray.direction * t - center) * inverseRadius};
        }

        inline int GetMaterialIndex() const noexcept override {
            return materialIndex;
        }
    };
}

#endif