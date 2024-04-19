#ifndef _BOX_H
#define _BOX_H

#include "Triangle.h"
#include "../math/Math.h"
#include "HittableObject.h"

namespace Shapes {
    class Box : public HittableObject {
    public:
        Triangle triangles[12];
        int materialIndex;
        inline Box(const Math::Vector3f &a, const Math::Vector3f &b, int materialIndex) noexcept :
            materialIndex(materialIndex) {            
            Math::Vector3f min(Math::Min(a.x, b.x), Math::Min(a.y, b.y), Math::Min(a.z, b.z));
            Math::Vector3f max(Math::Max(a.x, b.x), Math::Max(a.y, b.y), Math::Max(a.z, b.z));

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

        inline std::pair<float, Math::Vector3f> TryHit(const Ray &ray) const noexcept {
            float closestT = Math::Constants::Max<float>;
            Math::Vector3f worldNormal(0.f);
            for (int i = 0; i < 12; ++i) {
                auto [t, normal] = triangles[i].TryHit(ray);
                if (t < closestT) {
                    closestT = t;
                    worldNormal = normal;
                }
            }

            return {closestT, worldNormal};
        }

        inline int GetMaterialIndex() const noexcept {
            return materialIndex;
        }
    };
}

#endif