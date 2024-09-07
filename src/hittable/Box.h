#ifndef _BOX_H
#define _BOX_H

#include "Triangle.h"
#include "../Utilities.hpp"

namespace Shapes {
    //! Axis aligned parallelepiped. Consists of 12 triangles
    class Box final : public IHittable {
    public:
        Math::Vector3f min, max;
        AABB aabb;
        const Material *material;
        Triangle triangles[12];

        //! Constructs Box by default
        constexpr Box() noexcept = default;

        //! Constructs Box from ```min``` and ```max``` points with given pointer to Material
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

        //! Ray-Box intersection. Returns true if hit
        constexpr bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept override {
            bool anyHit = false;
            for (int i = 0; i < 12; ++i) {
                anyHit |= triangles[i].Hit(ray, tMin, Math::Min(tMax, payload.t), payload);
            }

            return anyHit;
        }

        //! Returns centroid of Box, i.e. average of ```min``` and ```max```
        constexpr Math::Vector3f GetCentroid() const noexcept override {
            return (aabb.min + aabb.max) * 0.5f;
        }

        //! Returns AABB of Box. Basically its bounds are box itself
        constexpr AABB GetBoundingBox() const noexcept override {
            return aabb;
        }

        //! Randomly chooses triangle and samples it.
        inline Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept override {
            return triangles[Utilities::RandomIntInRange(0, 12)].SampleUniform(sample);
        }

        //! Returns surface area of Box. Same as AABB's surface area
        constexpr float GetSurfaceArea() const noexcept override {
            float a = max.x - min.x;
            float b = max.y - min.y;
            float c = max.z - min.z;
            return 2.f * (a * b + b * c + a * c);
        }
    };
}

#endif
