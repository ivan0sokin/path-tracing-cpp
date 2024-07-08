#ifndef _POLYGON_H
#define _POLYGON_H

class Model;

#include "IHittable.h"
#include "../Material.h"

#include "Triangle.h"

//! Slightly differs from Triangle. It is connected with mesh and stores its face index
class Polygon : public IHittable {
public:
    constexpr Polygon(const Model *model, int meshIndex, int faceIndex) noexcept :
        m_Model(model), m_MeshIndex(meshIndex), m_FaceIndex(faceIndex) {}

    bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override;

    inline Math::Vector3f GetCentroid() const noexcept override {
        return MakeTriangle().GetCentroid();
    }

    inline AABB GetBoundingBox() const noexcept override {
        return MakeTriangle().GetBoundingBox();
    }

    inline Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept override {
        return MakeTriangle().SampleUniform(sample);
    }

    inline float GetSurfaceArea() const noexcept override {
        return MakeTriangle().GetSurfaceArea();
    }

private:
    Shapes::Triangle MakeTriangle() const noexcept;

private:
    const Model *m_Model;
    int m_MeshIndex;
    int m_FaceIndex;
};

#endif