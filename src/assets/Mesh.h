#ifndef _MESH_H
#define _MESH_H

#include "../math/Math.h"

#include <vector>
#include <span>
#include <cstdint>

//! Class that holds information about vertices in particular mesh in model
class Mesh {
public:
    //! Vertex information
    struct Vertex {
        Math::Vector3f position;
        Math::Vector3f normal;
        Math::Vector2f texcoord;
        Math::Vector3f tangent;

        constexpr Vertex() noexcept = default;

        friend constexpr bool operator==(const Vertex &a, const Vertex &b) noexcept {
            return a.position == b.position &&
                   a.normal == b.normal &&
                   a.texcoord == b.texcoord &&
                   a.tangent == b.tangent;
        }
    };

    Mesh(std::vector<Vertex> &&vertices, std::vector<int> &&indices, std::vector<int> &&materialIndices) noexcept : 
        m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), m_MaterialIndices(std::move(materialIndices)) {}

    inline int GetFaceCount() const noexcept {
        return static_cast<int>(m_Indices.size() / 3);
    }

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
        constexpr size_t operator()(const Mesh::Vertex &v) const noexcept {
            auto h1 = hash<Math::Vector3f>{}(v.position);
            auto h2 = hash<Math::Vector3f>{}(v.normal);
            auto h3 = hash<Math::Vector2f>{}(v.texcoord);
            auto h4 = hash<Math::Vector3f>{}(v.tangent);
            return Math::CombineHashes(Math::CombineHashes(h1, h2), Math::CombineHashes(h3, h4));
        }
    };
}

#endif