#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> &&vertices, std::vector<int> &&indices, std::vector<int> &&materialIndices) noexcept :
    m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), m_MaterialIndices(std::move(materialIndices)) {}
