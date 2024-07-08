#include "Model.h"

Model::Model(const std::filesystem::path &pathToFile, const std::filesystem::path &materialDirectory, std::vector<Mesh> &&meshes, std::vector<Material> &&materials) noexcept : 
    m_PathToFile(pathToFile), m_MaterialDirectory(materialDirectory), m_Meshes(std::move(meshes)), m_Materials(std::move(materials)) {
    // int totalVertexCount = 0;
    // for (const auto &mesh : m_Meshes) {
        // Math::Vector3f sum(0.f);
        // auto vertices = mesh.GetVertices();
        // for (const auto &vertex : vertices) {
            // sum += vertex.position;
        // }

        // totalVertexCount += static_cast<int>(vertices.size());
        // centroid += sum;
    // }

    // centroid /= static_cast<float>(totalVertexCount);

    int totalFaceCount = 0;
    for (const auto &mesh : m_Meshes) {
        totalFaceCount += static_cast<int>(mesh.GetIndices().size() / 3);
    }

    m_Polygons.reserve(totalFaceCount);
    for (int meshIndex = 0; meshIndex < (int)m_Meshes.size(); ++meshIndex) {
        const auto &mesh = m_Meshes[meshIndex];

        auto vertices = mesh.GetVertices();
        auto indices = mesh.GetIndices();
        auto materialIndices = mesh.GetMaterialIndices();

        int faceCount = static_cast<int>(indices.size() / 3);
        for (int faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
            auto &material = m_Materials[materialIndices[faceIndex]];
            m_Polygons.emplace_back(this, meshIndex, faceIndex);

            // if (material.emissionPower > 0.f) {
            //     m_LightSources.emplace_back(&m_Polygons.back());
            // }
        }
    }

    std::vector<IHittable*> hittables;
    for (auto &polygon : m_Polygons) {
        hittables.push_back(&polygon);
    }

    m_BVH = new BVH(hittables);
}

Model::~Model() noexcept {
    delete m_BVH;
}