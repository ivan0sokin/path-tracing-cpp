#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "../math/Math.h"
#include "HittableObject.h"

namespace Shapes {
    class Triangle : public HittableObject {
    public:
        Math::Vector3f vertices[3];
        Math::Vector3f edges[2];
        Math::Vector3f orientedNormal;
        int materialIndex;

        constexpr Triangle() = default;

        constexpr Triangle(const Math::Vector3f &a, const Math::Vector3f &b, const Math::Vector3f &c, int materialIndex) noexcept :
            vertices{a, b, c}, edges{b - a, c - a}, orientedNormal(Math::Normalize(Math::Cross(edges[0], edges[1]))), materialIndex(materialIndex) {}
    
        inline std::pair<float, Math::Vector3f> TryHit(const Ray &ray) const noexcept override {
            constexpr float epsilon = Math::Constants::Epsilon<float>;

            Math::Vector3f edge1 = edges[0];
            Math::Vector3f edge2 = edges[1];
            Math::Vector3f rayCrossEdge2 = Math::Cross(ray.direction, edge2);
            float determinant = Math::Dot(edge1, rayCrossEdge2);

            if (Math::Abs(determinant) < epsilon) {
                return {Math::Constants::Max<float>, Math::Vector3f(0.f)};
            }

            float inverseDeterminant = 1.f / determinant;
            Math::Vector3f s = ray.origin - vertices[0];
            float u = inverseDeterminant * Math::Dot(s, rayCrossEdge2);

            if (u < 0.f || u > 1.f) {
                return {Math::Constants::Max<float>, Math::Vector3f(0.f)};
            }

            Math::Vector3f sCrossEdge1 = Math::Cross(s, edge1);
            float v = inverseDeterminant * Math::Dot(ray.direction, sCrossEdge1);

            if (v < 0.f || u + v > 1.f) {
                return {Math::Constants::Max<float>, Math::Vector3f(0.f)};
            }

            float t = inverseDeterminant * Math::Dot(edge2, sCrossEdge1);

            return {t, orientedNormal};
        }

        inline int GetMaterialIndex() const noexcept override {
            return materialIndex;
        }
    };
}

#endif