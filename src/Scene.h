#ifndef _SCENE_H
#define _SCENE_H

#include "hittable/Sphere.h"
#include "hittable/Triangle.h"
#include "hittable/Box.h"
#include "Material.h"

#include <vector>

struct Scene {
    std::vector<Shapes::Sphere> spheres;
    std::vector<Shapes::Triangle> triangles;
    std::vector<Shapes::Box> boxes;
    std::vector<Material> materials;
};

#endif