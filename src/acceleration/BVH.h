#ifndef _BVH_H
#define _BVH_H

#include "../hittable/IHittable.h"

#include <vector>
#include <span>
#include <functional>

class BVH {
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

        //! Returns true if this node is leaf
        constexpr bool IsLeaf() const noexcept {
            return index <= 0;
        }
    };
    
public:
    inline BVH(std::span<IHittable* const> hittables) noexcept :
        m_Hittables(hittables.begin(), hittables.end()) {
        int n = static_cast<int>(hittables.size());
        m_Nodes.resize(2 * n);

        int usedNodes = 1;
        MakeHierarchySAH(1, 0, n, usedNodes);
    }

    inline bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept {
        const int TREE_DEPTH = 1024;

        int nodeIndex = 1;
        int nodeIndices[TREE_DEPTH];
        int stackPointer = 1;

        bool anyHit = false;
        while (stackPointer > 0) {
            if (m_Nodes[nodeIndex].IsLeaf()) {
                anyHit |= m_Hittables[-m_Nodes[nodeIndex].index]->Hit(ray, tMin, tMax, payload);
                tMax = Math::Min(tMax, payload.t);
                
                nodeIndex = nodeIndices[--stackPointer];
                continue;
            }

            int closestIndex = m_Nodes[nodeIndex].index;
            int furthestIndex = m_Nodes[nodeIndex].index | 1;

            float closestT = m_Nodes[closestIndex].aabb.Intersect(ray, tMin, tMax);
            float furtherT = m_Nodes[furthestIndex].aabb.Intersect(ray, tMin, tMax);

            if (closestT > furtherT) {
                std::swap(closestT, furtherT);
                std::swap(closestIndex, furthestIndex);
            }

            if (closestT == Math::Constants::Infinity<float>) {
                nodeIndex = nodeIndices[--stackPointer];
                continue;
            }

            nodeIndex = closestIndex;

            if (furtherT != Math::Constants::Infinity<float>) {
                nodeIndices[stackPointer++] = furthestIndex;
            }
        }

        return anyHit;
    }

    inline AABB GetBoundingBox() const noexcept {
        return m_Nodes[1].aabb;
    }

private:
    inline void MakeHierarchySAH(int index, int low, int high, int &usedNodes) noexcept {
        if (low + 1 == high) {
            m_Nodes[index] = Node(-low, m_Hittables[low]->GetBoundingBox());
            return;
        }
        
        int n = high - low;
        std::vector<AABB> pref(n + 1);
        std::vector<AABB> suff(n + 1);

        float minValue = Math::Constants::Infinity<float>;
        int mid = -1;
        int dimension = -1;

        for (int d = 0; d < 3; ++d) {
            std::sort(m_Hittables.begin() + low, m_Hittables.begin() + high, c_ComparatorsSAH[d]);

            pref[0] = AABB::Empty();
            for (int i = 0; i < n; ++i) {
                pref[i + 1] = AABB(pref[i], m_Hittables[i + low]->GetBoundingBox());
            }

            suff[n] = AABB::Empty();
            for (int i = n - 1; i >= 0; --i) {
                suff[i] = AABB(m_Hittables[i + low]->GetBoundingBox(), suff[i + 1]);
            }

            float minValueAlongAxis = Math::Constants::Infinity<float>;
            int index = -1;
            for (int i = 0; i < n; ++i) {
                float value = pref[i + 1].GetSurfaceArea() * (float)(i + 1) + suff[i + 1].GetSurfaceArea() * (float)(n - i - 1);
                if (value < minValueAlongAxis) {
                    minValueAlongAxis = value;
                    index = i + low;
                }
            }

            if (minValueAlongAxis < minValue) {
                minValue = minValueAlongAxis;
                mid = index + 1;
                dimension = d;
            }
        }

        std::sort(m_Hittables.begin() + low, m_Hittables.begin() + high, c_ComparatorsSAH[dimension]);
        
        int leftIndex = ++usedNodes;
        int rightIndex = ++usedNodes;
        MakeHierarchySAH(leftIndex, low, mid, usedNodes);
        MakeHierarchySAH(rightIndex, mid, high, usedNodes);

        m_Nodes[index] = Node(leftIndex, m_Nodes[leftIndex], m_Nodes[rightIndex]);
    }

private:
    std::vector<Node> m_Nodes;
    std::vector<const IHittable*> m_Hittables;

    inline static const std::function<bool(const IHittable*, const IHittable*)> c_ComparatorsSAH[3] = {
        [](const IHittable *a, const IHittable *b) {
            return a->GetCentroid().x < b->GetCentroid().x;
        },
        [](const IHittable *a, const IHittable *b) {
            return a->GetCentroid().y < b->GetCentroid().y;
        },
        [](const IHittable *a, const IHittable *b) {
            return a->GetCentroid().z < b->GetCentroid().z;
        }
    };
};

#endif