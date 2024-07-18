#ifndef _SPHERE_H
#define _SPHERE_H

#include "IHittable.h"

//! This namespace contains primitive geometric shapes
namespace Shapes {
    //! Sphere class. Holds information about its center, radius and material
    class Sphere : public IHittable {
    public:
        Math::Vector3f center;
        float radius, radiusSquared, inverseRadius;
        const Material *material;

        //! Constructs Sphere by default
        constexpr Sphere() noexcept = default;

        //! Constructs Sphere with given ```center```, ```radius``` and ```material```
        constexpr Sphere(const Math::Vector3f &center, float radius, const Material *material) noexcept :
            center(center), radius(radius), radiusSquared(radius * radius), inverseRadius(1.f / radius), material(material) {}

        //! Performs Ray-Sphere intersection. Returns true if hit
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

        //! Returns center of Sphere
        constexpr Math::Vector3f GetCentroid() const noexcept override {
            return center;
        }

        //! Returns AABB, which is constructed from ```center``` +- ```radius``` points
        constexpr AABB GetBoundingBox() const noexcept override {
            return AABB(center - radius, center + radius);
        }
        
        //! Samples Sphere surface uniformly. Returns point on surface
        constexpr Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept override {
            float z = 1.f - 2.f * sample.x;
            float r = Math::Sqrt(Math::Max(0.f, 1.f - z * z));
            float phi = Math::Constants::Tau<float> * sample.y;
            return center + radius * Math::Vector3f(r * Math::Cos(phi), r * Math::Sin(phi), z);
        }

        //! Returns surface area of Sphere
        constexpr float GetSurfaceArea() const noexcept override {
            return 4.f * Math::Constants::Pi<float> * radiusSquared;
        }
    };
}

#endif