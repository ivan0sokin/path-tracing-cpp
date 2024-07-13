#ifndef _ASSET_LODAER_H
#define _ASSET_LODAER_H

#include "Model.h"
#include "ModelInstance.h"
#include "../Texture.h"

#include "../../tinyobjloader/tiny_obj_loader.h"

#include <string>
#include <unordered_map>
#include <filesystem>
#include <utility>

class AssetLoader {
public:
    struct Result {
        std::string warning = "";
        std::string error = "";

        inline bool IsFailure() const noexcept {
            return !error.empty();
        }
    };

    struct LoadingProperties {
        bool generateSmoothNormals;
        bool surfaceAreaWeighting;
    };

public:
    AssetLoader(const AssetLoader&) = delete;
    AssetLoader(AssetLoader&&) = delete;
    AssetLoader& operator=(const AssetLoader&) = delete;
    AssetLoader& operator=(AssetLoader&&) = delete;

    inline static AssetLoader& Instance() noexcept {
        static AssetLoader assetLoader;
        return assetLoader;
    }

    ~AssetLoader() noexcept;

    constexpr std::span<Model* const> GetModels() noexcept {
        return m_Models;
    }

    constexpr LoadingProperties& GetLoadingProperties() noexcept {
        return m_LoadingProperties;
    }

    std::pair<ModelInstance*, Result> LoadOBJ(const std::filesystem::path &pathToFile, const std::filesystem::path &materialDirectory) noexcept;

    void IncreaseInstanceCount(int modelIndex) noexcept;

    void DecreaseInstanceCount(int modelIndex) noexcept;

private:
    Texture* LoadTexture(const std::filesystem::path &pathToTexture) noexcept;

    Material ProcessMaterial(const tinyobj::material_t &material, int index, const std::filesystem::path &materialDirectory) noexcept;

    Mesh* ProcessMesh(const tinyobj::attrib_t &attrib, const tinyobj::mesh_t &mesh) noexcept;

    Mesh::Vertex ProcessVertex(const tinyobj::attrib_t &attrib, const tinyobj::index_t &index) noexcept;

    void GenerateNormals(std::vector<Mesh::Vertex> &vertices, const std::vector<int> &indices, int faceCount) noexcept;

    void GenerateTangents(std::vector<Mesh::Vertex> &vertices, const std::vector<int> &indices, int faceCount) noexcept;

    void UnloadModel(int modelIndex) noexcept;

private:
    std::vector<Model*> m_Models;
    std::vector<int> m_InstanceCount;
    std::vector<std::vector<std::string>> m_TextureAbsolutePaths;

    std::unordered_map<std::string, Texture*> m_Textures;
    
    LoadingProperties m_LoadingProperties;

protected:
    AssetLoader() noexcept;
};

#endif