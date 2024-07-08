#ifndef _MESH_H
#define _MESH_H

#include "../math/Math.h"

#include <vector>
#include <span>

//! Class that holds information about vertices in particular mesh in model
class Mesh {
public:
    //! Vertex information
    struct Vertex {
        Math::Vector3f position;
        Math::Vector3f normal;
        Math::Vector2f texcoord;
    
        friend constexpr bool operator==(const Vertex &a, const Vertex &b) noexcept {
            return a.position == b.position && a.normal == b.normal && a.texcoord == b.texcoord;
        }
    };

    Mesh(std::vector<Vertex> &&vertices, std::vector<int> &&indices, std::vector<int> &&materialIndices) noexcept : 
        m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), m_MaterialIndices(std::move(materialIndices)) {}

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

#endif