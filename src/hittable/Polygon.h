#ifndef _POLYGON_H
#define _POLYGON_H

#include "HittableObject.h"
#include "Mesh.h"
#include "../Material.h"

#include "Triangle.h"

//! Slightly differs from Triangle. It is connected with mesh and stores its face index
class Polygon : public IHittable {
public:
    const Mesh *mesh;
    const Material *material;
    int faceIndex;

    constexpr Polygon(const Mesh *mesh, const Material *material, int faceIndex) noexcept :
        mesh(mesh), material(material), faceIndex(faceIndex) {}

    constexpr bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override {
        if (MakeTriangle().Hit(ray, tMin, tMax, payload)) {
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

            auto t0 = vertices[indices[3 * faceIndex + 0]].texcoord;
            auto t1 = vertices[indices[3 * faceIndex + 1]].texcoord;
            auto t2 = vertices[indices[3 * faceIndex + 2]].texcoord;

            Math::Vector3f p = ray.origin + ray.direction * payload.t;

            float u0 = Math::Length(Math::Cross(v1 - p, v2 - p)) / Math::Length(Math::Cross(v1 - v0, v2 - v0));
            float u1 = Math::Length(Math::Cross(v0 - p, v2 - p)) / Math::Length(Math::Cross(v1 - v0, v2 - v0));
            float u2 = 1.f - u1 - u0;

            payload.normal = Math::Normalize(n0 * u0 + n1 * u1 + n2 * u2);
            payload.texcoord = t0 * u0 + t1 * u1 + t2 * u2;
            // payload.normal = 2.f * material->bump.PickValue(payload.texcoord) - 1.f;

            return true;
        }

        return false;
    }

    constexpr Math::Vector3f GetCentroid() const noexcept override {
        return MakeTriangle().GetCentroid();
    }

    constexpr AABB GetBoundingBox() const noexcept override {
        return MakeTriangle().GetBoundingBox();
    }

    constexpr Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept override {
        return MakeTriangle().SampleUniform(sample);
    }

    constexpr float GetSurfaceArea() const noexcept override {
        return MakeTriangle().GetSurfaceArea();
    }

private:
    constexpr Shapes::Triangle MakeTriangle() const noexcept {
        auto vertices = mesh->GetVertices();
        auto indices = mesh->GetIndices();
        return Shapes::Triangle(vertices[indices[3 * faceIndex + 0]].position, vertices[indices[3 * faceIndex + 1]].position, vertices[indices[3 * faceIndex + 2]].position, material);
    }
};

#endif