#ifndef _SPHERE_H
#define _SPHERE_H

#include "../Material.h"
#include "IHittable.h"
#include "../math/Math.h"
#include "../AABB.h"
#include "../HitPayload.h"

namespace Shapes {
    class Sphere : public IHittable {
    public:
        Math::Vector3f center;
        float radius, radiusSquared, inverseRadius;
        const Material *material;

        constexpr Sphere() noexcept = default;

        constexpr Sphere(const Math::Vector3f &center, float radius, const Material *material) noexcept :
            center(center), radius(radius), radiusSquared(radius * radius), inverseRadius(1.f / radius), material(material) {}

        constexpr bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override {
            Math::Vector3f centerToOrigin = ray.origin - center;
        
            float a = Math::Dot(ray.direction, ray.direction);
            float k = Math::Dot(centerToOrigin, ray.direction);
            float c = Math::Dot(centerToOrigin, centerToOrigin) - radiusSquared;
            float discriminant = k * k - a * c;
            
            if (discriminant < 0.f) {
                return false;
            }

            float t0 = (-k - Math::Sqrt(discriminant)) / a;
            float t1 = (-k + Math::Sqrt(discriminant)) / a;

            float t = t0;
            if (t < tMin || tMax < t) {
                t = t1;
                if (t < tMin || tMax < t) {
                    return false;
                }
            }
            
            payload.t = t;
            payload.normal = (ray.origin + ray.direction * t - center) * inverseRadius;
            payload.material = material;

            return true;
        }

        constexpr Math::Vector3f GetCentroid() const noexcept override {
            return center;
        }

        constexpr AABB GetBoundingBox() const noexcept override {
            return AABB(center - radius, center + radius);
        }

        constexpr Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept override {
            float z = 1.f - 2.f * sample.x;
            float r = Math::Sqrt(Math::Max(0.f, 1.f - z * z));
            float phi = Math::Constants::Tau<float> * sample.y;
            return center + radius * Math::Vector3f(r * Math::Cos(phi), r * Math::Sin(phi), z);
        }

        constexpr float GetSurfaceArea() const noexcept override {
            return 4.f * Math::Constants::Pi<float> * radiusSquared;
        }
    };
}

#endif