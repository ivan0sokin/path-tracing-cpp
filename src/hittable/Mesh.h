#ifndef _MESH_H
#define _MESH_H

#include "../math/Math.h"

#include <vector>
#include <span>

class Model;

class Mesh {
public:
    struct Vertex {
        Math::Vector3f position;
        Math::Vector3f normal;
    
        friend constexpr bool operator==(const Vertex &a, const Vertex &b) noexcept {
            return a.position == b.position && a.normal == b.normal;
        }
    };

    Mesh(std::vector<Vertex> &&vertices, std::vector<int> &&indices, std::vector<int> &&materialIndices) noexcept;

    constexpr std::span<const Vertex> GetVertices() const noexcept {
        return m_Vertices;
    }

    constexpr std::span<const int> GetIndices() const noexcept {
        return m_Indices;
    }

    constexpr std::span<const int> GetMaterialIndices() const noexcept {
        return m_MaterialIndices;
    }

private:
    std::vector<Vertex> m_Vertices;
    std::vector<int> m_Indices;
    std::vector<int> m_MaterialIndices;
};

namespace std {
    template<>
    struct hash<Mesh::Vertex> {
        size_t operator()(const Mesh::Vertex &vertex) const noexcept {
            auto h1 = hash<Math::Vector3f>{}(vertex.position);
            auto h2 = hash<Math::Vector3f>{}(vertex.normal);
            return h1 ^ (h2 << 1);
        }
    };
}

#endif