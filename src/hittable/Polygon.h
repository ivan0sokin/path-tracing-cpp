#ifndef _POLYGON_H
#define _POLYGON_H

#include "HittableObject.h"
#include "Mesh.h"
#include "../Material.h"

#include "Triangle.h"


constexpr static Material mat = {
    Math::Vector3f(1.f),
    1.0f,
    0.f,
    0.2f,
    0.f,
    0
};

#include <cstdio>

class Polygon : public HittableObject {
public:
    const Mesh *mesh;
    const Material *material;
    int faceIndex;

    constexpr Polygon(const Mesh *mesh, const Material *material, int faceIndex) noexcept :
        mesh(mesh), material(material), faceIndex(faceIndex) {}

    inline void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override {
        // MakeTriangle().Hit(ray, tMin, tMax, payload);
        
        HitPayload hitPayload;
        hitPayload.t = Math::Constants::Infinity<float>;
        hitPayload.normal = Math::Vector3f(0.f);
        hitPayload.material = nullptr;

        MakeTriangle().Hit(ray, tMin, tMax, hitPayload);

        if (hitPayload.material != nullptr) {
            MakeTriangle().Hit(ray, tMin, tMax, payload);

            auto vertices = mesh->GetVertices();
            auto indices = mesh->GetIndices();

            auto v0 = vertices[indices[3 * faceIndex + 0]].position;
            auto v1 = vertices[indices[3 * faceIndex + 1]].position;
            auto v2 = vertices[indices[3 * faceIndex + 2]].position;

            auto n0 = vertices[indices[3 * faceIndex + 0]].normal;
            auto n1 = vertices[indices[3 * faceIndex + 1]].normal;
            auto n2 = vertices[indices[3 * faceIndex + 2]].normal;

            n0 = Math::Dot(ray.direction, n0) > Math::Constants::Epsilon<float> ? -n0 : n0;
            n1 = Math::Dot(ray.direction, n1) > Math::Constants::Epsilon<float> ? -n1 : n1;
            n2 = Math::Dot(ray.direction, n2) > Math::Constants::Epsilon<float> ? -n2 : n2;

            Math::Vector3f p = ray.origin + ray.direction * payload.t;

            float u0 = Math::Length(Math::Cross(v1 - p, v2 - p)) / Math::Length(Math::Cross(v1 - v0, v2 - v0));
            float u1 = Math::Length(Math::Cross(v0 - p, v2 - p)) / Math::Length(Math::Cross(v1 - v0, v2 - v0));
            float u2 = 1.f - u1 - u0;

            payload.normal = Math::Normalize(n0 * u0 + n1 * u1 + n2 * u2);
        }
    }

    inline Math::Vector3f GetCentroid() const noexcept override {
        return MakeTriangle().GetCentroid();
    }

    inline AABB GetBoundingBox() const noexcept override {
        return MakeTriangle().GetBoundingBox();
    }

private:
    inline Shapes::Triangle MakeTriangle() const noexcept {
        auto vertices = mesh->GetVertices();
        auto indices = mesh->GetIndices();
        return Shapes::Triangle(vertices[indices[3 * faceIndex + 0]].position, vertices[indices[3 * faceIndex + 1]].position, vertices[indices[3 * faceIndex + 2]].position, material);
    }
};

#endif