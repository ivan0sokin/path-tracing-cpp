#ifndef _POLYGON_H
#define _POLYGON_H

#include "../assets/Model.h"
#include "Triangle.h"

//! Primitive element of Mesh
class Polygon : public IHittable {
public:
    constexpr Polygon(const Model *model, int meshIndex, int faceIndex) noexcept :
        m_Model(model), m_MeshIndex(meshIndex), m_FaceIndex(faceIndex) {}

    //! Performs Ray-Triangle intersection with all model stuff like tangents, texture coordinates and weighted normals
    bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override;

    //! Returns centroid of Triangle
    constexpr Math::Vector3f GetCentroid() const noexcept override {
        return MakeTriangle().GetCentroid();
    }

    //! Returns AABB of Triangle
    constexpr AABB GetBoundingBox() const noexcept override {
        return MakeTriangle().GetBoundingBox();
    }

    //! Returns point on surface of Triangle
    constexpr Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept override {
        return MakeTriangle().SampleUniform(sample);
    }

    //! Returns surface area of Triangle
    constexpr float GetSurfaceArea() const noexcept override {
        return MakeTriangle().GetSurfaceArea();
    }

private:
    constexpr Shapes::Triangle MakeTriangle() const noexcept {
        auto mesh = m_Model->GetMeshes()[m_MeshIndex];
    
        auto vertices = mesh->GetVertices();
        auto indices = mesh->GetIndices();

        auto materials = m_Model->GetMaterials();
        auto materialIndices = mesh->GetMaterialIndices();
            
        return Shapes::Triangle(vertices[indices[3 * m_FaceIndex + 0]].position, vertices[indices[3 * m_FaceIndex + 1]].position, vertices[indices[3 * m_FaceIndex + 2]].position, &materials[materialIndices[m_FaceIndex]]);
    }

private:
    const Model *m_Model;
    int m_MeshIndex;
    int m_FaceIndex;
};

#endif