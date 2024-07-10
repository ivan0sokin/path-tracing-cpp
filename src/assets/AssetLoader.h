#ifndef _ASSET_LODAER_H
#define _ASSET_LODAER_H

#include "Model.h"
#include "ModelInstance.h"

#include <string>
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

    std::pair<ModelInstance*, Result> LoadOBJ(const std::filesystem::path &pathToFile, const std::filesystem::path &materialDirectory) noexcept;

    void IncreaseInstanceCount(int modelIndex) noexcept;

    void DecreaseInstanceCount(int modelIndex) noexcept;

private:
    std::vector<Model*> m_Models;
    std::vector<int> m_InstanceCount;

protected:
    AssetLoader() noexcept = default;
};

#endif