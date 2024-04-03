#ifndef _SCENE_H
#define _SCENE_H

#include "Sphere.h"
#include "Material.h"

#include <vector>

struct Scene {
    std::vector<Sphere> spheres;
    std::vector<Material> materials;
};

#endif