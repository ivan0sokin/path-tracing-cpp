#ifndef _SCENE_H
#define _SCENE_H

#include "Sphere.h"
#include "Triangle.h"
#include "Material.h"

#include <vector>

struct Scene {
    std::vector<Sphere> spheres;
    std::vector<Triangle> triangles;
    std::vector<Material> materials;
};

#endif