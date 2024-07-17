#ifndef _TLAS_H
#define _TLAS_H

#include <vector>

#include "BLAS.h"
#include "../Utilities.hpp"

//! Top-level acceleration structure. Used to combine multiple BLAS in one structure, also binary tree structured.
class TLAS {
private:
    struct Node {
        int index;
        AABB aabb;

        constexpr Node() noexcept :
            index(-1), aabb(AABB::Empty()) {}

        constexpr Node(int index, const AABB &aabb) noexcept :
            index(index), aabb(aabb) {}

        constexpr Node(int index, const Node &left, const Node &right) noexcept :
            index(index), aabb(left.aabb, right.aabb) {}

        constexpr bool IsLeaf() const noexcept {
            return index <= 0;
        }
    };

public:
    //! Constructs TLAS with given span of BLAS
    inline TLAS(std::span<BLAS* const> blas) noexcept :
        m_BLAS(blas.begin(), blas.end()) {
        int n = static_cast<int>(blas.size());
        m_Nodes.resize(2 * n);

        int usedNodes = 1;
        MakeHierarchyNaive(1, 0, n, usedNodes);
    }

    //! Performs worldray-TLAS intersection
    inline bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept {
        if (m_Nodes[1].aabb.Intersect(ray, tMin, tMax) == Math::Constants::Infinity<float>) {
            return false;
        }

        const int TREE_DEPTH = 1024;

        int nodeIndex = 1;
        int nodeIndices[TREE_DEPTH];

        int stackPointer = 1;

        bool anyHit = false;
        while (stackPointer > 0) {
            if (m_Nodes[nodeIndex].IsLeaf()) {
                int blasIndex = -m_Nodes[nodeIndex].index;
                anyHit |= m_BLAS[blasIndex]->Hit(ray, tMin, tMax, payload);
                tMax = Math::Min(tMax, payload.t);
                
                nodeIndex = nodeIndices[--stackPointer];
                continue;
            }

            int closestIndex = m_Nodes[nodeIndex].index;
            int furthestIndex = m_Nodes[nodeIndex].index | 1;

            float closestT = m_Nodes[closestIndex].aabb.Intersect(ray, tMin, tMax);
            float furthestT = m_Nodes[furthestIndex].aabb.Intersect(ray, tMin, tMax);

            if (closestT > furthestT) {
                std::swap(closestT, furthestT);
                std::swap(closestIndex, furthestIndex);
            }

            if (closestT == Math::Constants::Infinity<float>) {
                nodeIndex = nodeIndices[--stackPointer];
                continue;
            }

            nodeIndex = closestIndex;

            if (furthestT != Math::Constants::Infinity<float>) {
                nodeIndices[stackPointer++] = furthestIndex;
            }
        }

        return anyHit;
    }

private:
    inline void MakeHierarchyNaive(int index, int low, int high, int &usedNodes) noexcept {
        if (low + 1 == high) {
            m_Nodes[index] = Node(-low, m_BLAS[low]->GetLocalBoundingBox());
            return;
        }

        std::shuffle(m_BLAS.begin(), m_BLAS.end(), Utilities::s_RandomNumberGenerator);

        int leftIndex = ++usedNodes;
        int rightIndex = ++usedNodes;
        int mid = (low + high) / 2;
        MakeHierarchyNaive(leftIndex, low, mid, usedNodes);
        MakeHierarchyNaive(rightIndex, mid, high, usedNodes);

        m_Nodes[index] = Node(leftIndex, m_Nodes[leftIndex], m_Nodes[rightIndex]);
    }

private:
    std::vector<Node> m_Nodes;
    std::vector<const BLAS*> m_BLAS;
};

#endif