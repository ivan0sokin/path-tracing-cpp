#ifndef _MODEL_H
#define _MODEL_H

#include "HittableObject.h"
#include "Mesh.h"
#include "../BVHNode.h"
#include "Polygon.h"

#include <vector>
#include <filesystem>
#include <string>

class Model : public HittableObject {
public:
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

    void Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept;

    Math::Vector3f GetCentroid() const noexcept;

    AABB GetBoundingBox() const noexcept;

    std::filesystem::path GetPathToFile() const noexcept {
        return m_PathToFile;
    }

    std::filesystem::path GetMaterialDirectory() const noexcept {
        return m_MaterialDirectory;
    }

private:
    Model(const std::filesystem::path &pathToFile, const std::filesystem::path &materialDirectory, std::vector<Mesh> &&meshes, std::vector<Material> &&materials) noexcept;

private:
    const std::filesystem::path m_PathToFile;
    const std::filesystem::path m_MaterialDirectory;

    std::vector<Mesh> m_Meshes;
    std::vector<Polygon> m_Polygons;
    std::vector<Material> m_Materials;

    BVHNode *m_Root;

    Math::Vector3f centroid;

    constexpr static int c_VerticesPerFace = 3;
};

#endif