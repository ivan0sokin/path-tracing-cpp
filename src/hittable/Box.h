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
        AABB aabb;
        const Material *material;
        Triangle triangles[12];

        constexpr Box() noexcept = default;

        constexpr Box(const Math::Vector3f &min, const Math::Vector3f &max, const Material *material) noexcept : 
            min(min), max(max), aabb(min, max), material(material) {   
            triangles[0] = Triangle({min.x, min.y, max.z}, {min.x, min.y, min.z}, {max.x, min.y, min.z}, this->material); // bottom
            triangles[1] = Triangle({max.x, min.y, min.z}, {max.x, min.y, max.z}, {min.x, min.y, max.z}, this->material);
            triangles[2] = Triangle({min.x, min.y, min.z}, {min.x, max.y, min.z}, {max.x, max.y, min.z}, this->material); // back
            triangles[3] = Triangle({max.x, max.y, min.z}, {max.x, min.y, min.z}, {min.x, min.y, min.z}, this->material);
            triangles[4] = Triangle({max.x, max.y, max.z}, {max.x, max.y, min.z}, {min.x, max.y, min.z}, this->material); // up
            triangles[5] = Triangle({min.x, max.y, min.z}, {min.x, max.y, max.z}, {max.x, max.y, max.z}, this->material);
            triangles[6] = Triangle({min.x, min.y, max.z}, {max.x, min.y, max.z}, {max.x, max.y, max.z}, this->material); // front
            triangles[7] = Triangle({max.x, max.y, max.z}, {min.x, max.y, max.z}, {min.x, min.y, max.z}, this->material);
            triangles[8] = Triangle({min.x, min.y, max.z}, {min.x, max.y, max.z}, {min.x, max.y, min.z}, this->material); // left
            triangles[9] = Triangle({min.x, max.y, min.z}, {min.x, min.y, min.z}, {min.x, min.y, max.z}, this->material);
            triangles[10] = Triangle({max.x, min.y, min.z}, {max.x, max.y, min.z}, {max.x, max.y, max.z}, this->material); // right
            triangles[11] = Triangle({max.x, max.y, max.z}, {max.x, min.y, max.z}, {max.x, min.y, min.z}, this->material);
        }

        constexpr bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override {
            bool anyHit = false;
            for (int i = 0; i < 12; ++i) {
                anyHit |= triangles[i].Hit(ray, tMin, Math::Min(tMax, payload.t), payload);
            }

            return anyHit;
        }

        constexpr Math::Vector3f GetCentroid() const noexcept override {
            return (aabb.min + aabb.max) * 0.5f;
        }

        constexpr AABB GetBoundingBox() const noexcept override {
            return aabb;
        }

        constexpr Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept override {
            return Math::Vector3f(0.f);
        }

        constexpr float GetArea() const noexcept override {
            return 0.f;
        }
    };
}

#endif