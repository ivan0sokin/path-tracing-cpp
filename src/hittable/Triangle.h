#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "../math/Math.h"
#include "HittableObject.h"
#include "../AABB.h"
#include "../HitPayload.h"

#include <array>

namespace Shapes {
    class Triangle : public HittableObject {
    public:
        Math::Vector3f vertices[3];
        Math::Vector3f edges[2];
        Math::Vector3f normal;
        int materialIndex;

        constexpr Triangle() = default;

        constexpr Triangle(const Math::Vector3f &a, const Math::Vector3f &b, const Math::Vector3f &c, int materialIndex) noexcept :
            vertices{a, b, c}, edges{b - a, c - a}, normal(Math::Normalize(Math::Cross(edges[0], edges[1]))), materialIndex(materialIndex) {}

        constexpr Triangle(const std::array<Math::Vector3f, 3> &vertices, const Math::Vector3f &normal, int materialIndex) noexcept :
            vertices{vertices[0], vertices[1], vertices[2]}, edges{vertices[1] - vertices[0], vertices[2] - vertices[0]}, normal(normal), materialIndex(materialIndex) {}

        inline void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override {
            constexpr float epsilon = Math::Constants::Epsilon<float>;

            Math::Vector3f rayCrossEdge2 = Math::Cross(ray.direction, edges[1]);
            float determinant = Math::Dot(edges[0], rayCrossEdge2);

            if (Math::Abs(determinant) < epsilon) {
                return;
            }

            float inverseDeterminant = 1.f / determinant;
            Math::Vector3f s = ray.origin - vertices[0];
            float u = inverseDeterminant * Math::Dot(s, rayCrossEdge2);

            if (u < 0.f || u > 1.f) {
                return;
            }

            Math::Vector3f sCrossEdge1 = Math::Cross(s, edges[0]);
            float v = inverseDeterminant * Math::Dot(ray.direction, sCrossEdge1);

            if (v < 0.f || u + v > 1.f) {
                return;
            }

            float t = inverseDeterminant * Math::Dot(edges[1], sCrossEdge1);

            if (t < tMin || tMax < t) {
                return;
            }

            payload.t = t;
            payload.normal = normal;
            payload.materialIndex = materialIndex;
        }

        inline int GetMaterialIndex() const noexcept override {
            return materialIndex;
        }

        inline AABB GetBoundingBox() const noexcept override {
            return AABB(Math::Min(vertices[0], Math::Min(vertices[1], vertices[2])) - 0.1f,
                        Math::Max(vertices[0], Math::Max(vertices[1], vertices[2])) + 0.1f);
        }
    };
}

#endif