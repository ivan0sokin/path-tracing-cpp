#ifndef _POLYGON_H
#define _POLYGON_H

#include "../assets/Model.h"
#include "Triangle.h"

//! Primitive element of Mesh
class Polygon final : public IHittable {
public:
    constexpr Polygon(const Model *model, const Mesh *mesh, int faceIndex) noexcept :
        m_Model(model), m_Mesh(mesh), m_FaceIndex(faceIndex) {
        auto vertices = mesh->GetVertices();
        auto indices = mesh->GetIndices();

        const auto &p0 = vertices[indices[3 * m_FaceIndex + 0]].position;
        const auto &p1 = vertices[indices[3 * m_FaceIndex + 1]].position;
        const auto &p2 = vertices[indices[3 * m_FaceIndex + 2]].position;

        m_Centroid = (p0 + p1 + p2) * Math::Constants::OneThird<float>;
        m_AABB = AABB(Math::Min(p0, Math::Min(p1, p2)), Math::Max(p0, Math::Max(p1, p2)));
        m_TwiceSurfaceArea = Math::Length(Math::Cross(p1 - p0, p2 - p0));
        m_SurfaceArea = m_TwiceSurfaceArea * 0.5f;
        m_Edges[0] = p1 - p0;
        m_Edges[1] = p2 - p0;
    }

    //! Performs Ray-Triangle intersection with all model stuff like tangents, texture coordinates and weighted normals
    bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override;

    //! Returns centroid of Triangle
    constexpr Math::Vector3f GetCentroid() const noexcept override {
        return m_Centroid;
    }

    //! Returns AABB of Triangle
    constexpr AABB GetBoundingBox() const noexcept override {
        return m_AABB;
    }

    //! Returns point on surface of Triangle
    constexpr Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept override {
        float sqrt = Math::Sqrt(sample.x);
        float b0 = 1.f - sqrt;
        float b1 = sample.y * sqrt;

        auto vertices = m_Mesh->GetVertices();
        auto indices = m_Mesh->GetIndices();

        const auto &p0 = vertices[indices[3 * m_FaceIndex + 0]].position;
        const auto &p1 = vertices[indices[3 * m_FaceIndex + 1]].position;
        const auto &p2 = vertices[indices[3 * m_FaceIndex + 2]].position;

        return b0 * p0 + b1 * p1 + (1.f - b0 - b1) * p2;
    }

    //! Returns surface area of Triangle
    constexpr float GetSurfaceArea() const noexcept override {
        return m_SurfaceArea;
    }

private:
    constexpr std::tuple<float, float, float> ComputeBarycentrics(std::span<const Math::Vector3f> pointToVertices) const noexcept {
        float u0 = Math::Length(Math::Cross(pointToVertices[1], pointToVertices[2])) / m_TwiceSurfaceArea;
        float u1 = Math::Length(Math::Cross(pointToVertices[0], pointToVertices[2])) / m_TwiceSurfaceArea;
        return {u0, u1, 1.f - u0 - u1};
    }

private:
    const Model *m_Model;
    const Mesh *m_Mesh;
    int m_FaceIndex;

    Math::Vector3f m_Centroid;
    AABB m_AABB;
    float m_TwiceSurfaceArea;
    float m_SurfaceArea;
    Math::Vector3f m_Edges[2];
};

#endif
