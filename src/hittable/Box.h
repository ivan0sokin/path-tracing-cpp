#ifndef _BOX_H
#define _BOX_H

#include "../math/Math.h"
#include "Triangle.h"
#include "HittableObject.h"
#include "../HitPayload.h" 
#include "../AABB.h"

namespace Shapes {
    class Box : public HittableObject {
    public:
        Math::Vector3f min, max;
        Triangle triangles[12];
        AABB aabb;
        int materialIndex;

        constexpr Box() noexcept = default;

        constexpr Box(const Math::Vector3f &min, const Math::Vector3f &max, int materialIndex) noexcept : 
            aabb(min, max), materialIndex(materialIndex), min(min), max(max) {            
            triangles[0] = Triangle({min.x, min.y, max.z}, {min.x, min.y, min.z}, {max.x, min.y, min.z}, materialIndex); // bottom
            triangles[1] = Triangle({max.x, min.y, min.z}, {max.x, min.y, max.z}, {min.x, min.y, max.z}, materialIndex);
            triangles[2] = Triangle({min.x, min.y, min.z}, {min.x, max.y, min.z}, {max.x, max.y, min.z}, materialIndex); // back
            triangles[3] = Triangle({max.x, max.y, min.z}, {max.x, min.y, min.z}, {min.x, min.y, min.z}, materialIndex);
            triangles[4] = Triangle({max.x, max.y, max.z}, {max.x, max.y, min.z}, {min.x, max.y, min.z}, materialIndex); // up
            triangles[5] = Triangle({min.x, max.y, min.z}, {min.x, max.y, max.z}, {max.x, max.y, max.z}, materialIndex);
            triangles[6] = Triangle({min.x, min.y, max.z}, {max.x, min.y, max.z}, {max.x, max.y, max.z}, materialIndex); // front
            triangles[7] = Triangle({max.x, max.y, max.z}, {min.x, max.y, max.z}, {min.x, min.y, max.z}, materialIndex);
            triangles[8] = Triangle({min.x, min.y, max.z}, {min.x, max.y, max.z}, {min.x, max.y, min.z}, materialIndex); // left
            triangles[9] = Triangle({min.x, max.y, min.z}, {min.x, min.y, min.z}, {min.x, min.y, max.z}, materialIndex);
            triangles[10] = Triangle({max.x, min.y, min.z}, {max.x, max.y, min.z}, {max.x, max.y, max.z}, materialIndex); // right
            triangles[11] = Triangle({max.x, max.y, max.z}, {max.x, min.y, max.z}, {max.x, min.y, min.z}, materialIndex);
        }

        constexpr void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override {
            for (int i = 0; i < 12; ++i) {
                triangles[i].Hit(ray, tMin, Math::Min(tMax, payload.t), payload);
            }
        }

        constexpr Math::Vector3f GetCentroid() const noexcept override {
            return (aabb.min + aabb.max) * 0.5f;
        }

        constexpr AABB GetBoundingBox() const noexcept override {
            return aabb;
        }
    };
}

#endif