#ifndef _ACCELERATION_STRUCTURE_H
#define _ACCELERATION_STRUCTURE_H

#include "Scene.h"

class AccelerationStructure {
public:
    AccelerationStructure(const Scene &scene) noexcept;

private:
    struct BVHNode {
        BVHNode *left = nullptr, *right = nullptr;

        constexpr bool IsTerminating() const noexcept {
            return left == nullptr;
        }
    };

private:
    BVHNode *root = nullptr;
};

#endif