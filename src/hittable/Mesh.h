#ifndef _MESH_H
#define _MESH_H

#include "../BVHNode.h"
#include "Triangle.h"
#include "HittableObject.h"

#include <vector>

class Mesh : public HittableObject {
public:
    Mesh(const char *pathToFile, const char *materialDirectory) noexcept;

    void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override;

    int GetMaterialIndex() const noexcept override;

    AABB GetBoundingBox() const noexcept override;

private:
    BVHNode *MakeHierarchy() const noexcept;

private:
    std::vector<Shapes::Triangle> m_Triangles;
    Math::Vector3f m_Min = Math::Vector3f(Math::Constants::Infinity<float>), m_Max = Math::Vector3f(-Math::Constants::Infinity<float>);
    BVHNode *m_Root = nullptr;
};

#endif