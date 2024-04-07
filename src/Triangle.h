#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "../glm/include/glm/vec3.hpp"
#include "../glm/include/glm/geometric.hpp"

struct Triangle {
    glm::vec3 vertices[3];
    glm::vec3 edges[2];
    glm::vec3 normal;
    int materialIndex;

    constexpr Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, int materialIndex) noexcept :
        vertices{a, b, c}, edges{b - a, c - a}, normal(glm::cross(edges[0], edges[1])), materialIndex(materialIndex) {}
};

#endif