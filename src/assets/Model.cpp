#include "Model.h"

Model::Model(const std::filesystem::path &pathToFile, const std::filesystem::path &materialDirectory, std::vector<Mesh*> &&meshes, std::vector<Material> &&materials, int totalFaceCount) noexcept : 
    m_PathToFile(pathToFile), m_MaterialDirectory(materialDirectory), m_Meshes(std::move(meshes)), m_Materials(std::move(materials)) {
    m_Polygons.reserve(totalFaceCount);
    for (int meshIndex = 0; meshIndex < static_cast<int>(m_Meshes.size()); ++meshIndex) {
        const auto &mesh = m_Meshes[meshIndex];

        int faceCount = mesh->GetFaceCount();
        for (int faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
            m_Polygons.emplace_back(this, meshIndex, faceIndex);
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

    for (auto mesh : m_Meshes) {
        delete mesh;
    }
}