#ifndef _MODEL_H
#define _MODEL_H

#include "HittableObject.h"
#include "Mesh.h"
#include "../BVHNode.h"
#include "Polygon.h"
#include "../Light.h"

#include <vector>
#include <filesystem>
#include <string>

//! Class that allows to load .obj models and store them
class Model : public IHittable {
public:
    //! Model loading result
    struct LoadResult {
        Model *model = nullptr;
        std::string warning = "";
        std::string error = "";

        inline static LoadResult Error(const std::string &error) noexcept {
            LoadResult result;
            result.error = error;
            return result;
        }

        inline bool IsFailure() noexcept {
            return !error.empty();
        }
    };

public:
    static LoadResult LoadOBJ(const std::filesystem::path &pathToFile, const std::filesystem::path &materialDirectory) noexcept;

    bool Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept;

    Math::Vector3f GetCentroid() const noexcept;

    AABB GetBoundingBox() const noexcept;

    constexpr Math::Vector3f SampleUniform(const Math::Vector2f &sample) const noexcept override {
        return Math::Vector3f(0.f);
    }

    constexpr float GetArea() const noexcept override {
        return 0.f;
    }

    std::filesystem::path GetPathToFile() const noexcept {
        return m_PathToFile;
    }

    std::filesystem::path GetMaterialDirectory() const noexcept {
        return m_MaterialDirectory;
    }

    std::span<Material> GetMaterials() noexcept {
        return m_Materials;
    }

    std::span<const Light> GetLightSources() const noexcept {
        return m_LightSources;
    }

private:
    Model(const std::filesystem::path &pathToFile, const std::filesystem::path &materialDirectory, std::vector<Mesh> &&meshes, std::vector<Material> &&materials, std::vector<std::vector<uint32_t>> &&textures) noexcept;

private:
    const std::filesystem::path m_PathToFile;
    const std::filesystem::path m_MaterialDirectory;

    std::vector<Mesh> m_Meshes;
    std::vector<Polygon> m_Polygons;
    std::vector<Material> m_Materials;
    std::vector<Light> m_LightSources;

    BVHNode *m_Root;

    Math::Vector3f centroid;

    constexpr static int c_VerticesPerFace = 3;
};

#endif