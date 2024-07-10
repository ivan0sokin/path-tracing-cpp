#ifndef _MODEL_H
#define _MODEL_H

#include "Mesh.h"
#include "../hittable/Polygon.h"
#include "../Light.h"
#include "../acceleration/BVH.h"

#include <vector>
#include <filesystem>
#include <string>

//! Class that holds information about .obj model
class Model {
public:
    Model(const std::filesystem::path &pathToFile, const std::filesystem::path &materialDirectory, std::vector<Mesh> &&meshes, std::vector<Material> &&materials, int totalFaceCount) noexcept;

    ~Model() noexcept;

    constexpr std::span<const Mesh> GetMeshes() const noexcept {
        return m_Meshes;
    }

    constexpr std::span<const Material> GetMaterials() const noexcept {
        return m_Materials;
    }

    constexpr std::span<const Light> GetLightSources() const noexcept {
        return m_LightSources;
    }

    constexpr const BVH* GetBVH() const noexcept {
        return m_BVH;
    }

    inline std::filesystem::path GetPathToFile() const noexcept {
        return m_PathToFile;
    }

    inline std::filesystem::path GetMaterialDirectory() const noexcept {
        return m_MaterialDirectory;
    }

private:
    const std::filesystem::path m_PathToFile;
    const std::filesystem::path m_MaterialDirectory;

    std::vector<Mesh> m_Meshes;
    std::vector<Polygon> m_Polygons;
    std::vector<Material> m_Materials;
    std::vector<Light> m_LightSources;

    BVH *m_BVH;
};

#endif