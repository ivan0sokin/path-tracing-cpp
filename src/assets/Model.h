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
    //! Constructs model with given parameters
    Model(const std::filesystem::path &pathToFile, const std::filesystem::path &materialDirectory, std::vector<Mesh*> &&meshes, std::vector<Material> &&materials, int totalFaceCount) noexcept;

    ~Model() noexcept;

    //! Returns span of meshes
    constexpr std::span<Mesh* const> GetMeshes() const noexcept {
        return m_Meshes;
    }

    //! Returns span of materials
    constexpr std::span<const Material> GetMaterials() const noexcept {
        return m_Materials;
    }

    //! Returns span of light sources
    constexpr std::span<const Light> GetLightSources() const noexcept {
        return m_LightSources;
    }

    //! Returns pointer to BVH of the model
    constexpr const BVH* GetBVH() const noexcept {
        return m_BVH;
    }

    //! Returns path to .obj model file
    inline std::filesystem::path GetPathToFile() const noexcept {
        return m_PathToFile;
    }

    //! Returns path to material directory
    inline std::filesystem::path GetMaterialDirectory() const noexcept {
        return m_MaterialDirectory;
    }

private:
    const std::filesystem::path m_PathToFile;
    const std::filesystem::path m_MaterialDirectory;

    std::vector<Mesh*> m_Meshes;
    std::vector<Polygon> m_Polygons;
    std::vector<Material> m_Materials;
    std::vector<Light> m_LightSources;

    BVH *m_BVH;
};

#endif