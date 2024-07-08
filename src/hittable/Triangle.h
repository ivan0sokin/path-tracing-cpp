#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "../math/Math.h"
#include "IHittable.h"
#include "../AABB.h"
#include "../HitPayload.h"

#include <array>
#include <cstdio>

namespace Shapes {
    class Triangle : public IHittable {
    public:
        Math::Vector3f vertices[3];
        Math::Vector3f edges[2];
        Math::Vector3f normal;
        const Material *material;

        constexpr Triangle() = default;

        constexpr Triangle(const Math::Vector3f &a, const Math::Vector3f &b, const Math::Vector3f &c, const Material *material) noexcept :
            vertices{a, b, c}, edges{b - a, c - a}, normal(Math::Normalize(Math::Cross(edges[0], edges[1]))), material(material) {}

        constexpr Triangle(const std::array<Math::Vector3f, 3> &vertices, const Math::Vector3f &normal, const Material *material) noexcept :
            vertices{vertices[0], vertices[1], vertices[2]}, edges{vertices[1] - vertices[0], vertices[2] - vertices[0]}, normal(normal), material(material) {}

        constexpr bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override {
            Math::Vector3f rayCrossEdge2 = Math::Cross(ray.direction, edges[1]);
            float determinant = Math::Dot(edges[0], rayCrossEdge2);

            if (Math::Abs(determinant) < Math::Constants::Epsilon<float>) {
                return false;
            }

            float inverseDeterminant = 1.f / determinant;
            Math::Vector3f s = ray.origin - vertices[0];
            float u = inverseDeterminant * Math::Dot(s, rayCrossEdge2);

            if (u < 0.f || u > 1.f) {
                return false;
            }

            Math::Vector3f sCrossEdge1 = Math::Cross(s, edges[0]);
            float v = inverseDeterminant * Math::Dot(ray.direction, sCrossEdge1);

            if (v < 0.f || u + v > 1.f) {
                return false;
            }

            float t = inverseDeterminant * Math::Dot(edges[1], sCrossEdge1);

            if (t < tMin || tMax < t) {
                return false;
            }

            payload.t = t;
            payload.normal = normal;
            payload.material = material;

            return true;
        }

        constexpr Math::Vector3f GetCentroid() const noexcept override {
            return (vertices[0] + vertices[1] + vertices[2]) * Math::Constants::OneThird<float>;
        }

        constexpr AABB GetBoundingBox() const noexcept override {
            return AABB(Math::Min(vertices[0], Math::Min(vertices[1], vertices[2])) - 0.01f,
                        Math::Max(vertices[0], Math::Max(vertices[1], vertices[2])) + 0.01f);
        }

        constexpr Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept override {
            float sqrt = Math::Sqrt(sample.x);
            float b0 = 1.f - sqrt;
            float b1 = sample.y * sqrt;

            return b0 * vertices[0] + b1 * vertices[1] + (1.f - b0 - b1) * vertices[2];
        }

        constexpr float GetSurfaceArea() const noexcept override {
            return Math::Length(Math::Cross(edges[0], edges[1])) * 0.5f;
        }   
    };
}

#endif