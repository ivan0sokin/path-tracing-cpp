#ifndef _SCENE_H
#define _SCENE_H

#include "Material.h"
#include "hittable/HittableObject.h"

#include <vector>

struct Scene {
    std::vector<HittableObject*> objects;
    std::vector<HittableObject*> lights;
    std::vector<Material> materials;
};

#endif