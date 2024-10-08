#ifndef _SCENE_H
#define _SCENE_H

#include "hittable/Sphere.h"
#include "hittable/Triangle.h"
#include "hittable/Box.h"
#include "assets/AssetLoader.h"
#include "Material.h"
#include "Camera.h"
#include "Timer.h"

#include <vector>
#include <fstream>
#include <array>
#include <exception>
#include <optional>

//! Struct that holds camera, all primitives and models. Can be serialized/deserialized
struct Scene {
    std::vector<Shapes::Sphere> spheres;
    std::vector<Shapes::Triangle> triangles;
    std::vector<Shapes::Box> boxes;
    std::vector<ModelInstance*> modelInstances;
    std::vector<Material> materials;
    Camera camera;

    //! Serializes scene into ```std::ostream```
    std::optional<std::string> Serialize(std::ostream &os) const noexcept {
        os.exceptions(std::ios::badbit | std::ios::failbit);

        try {
            TrySerialize(os);
        } catch (std::exception &e) {
            return e.what();
        }

        return {};
    }

    //! Deserializes scene from ```std::istream```
    std::optional<std::string> Deserialize(std::istream &is) noexcept {
        is.exceptions(std::ios::eofbit | std::ios::badbit | std::ios::failbit);

        try {
            TryDeserialize(is);
        } catch (std::exception &e) {
            return e.what();
        }

        return {};
    }

private:
    void TrySerialize(std::ostream &os) const {
        int loadedModelCount = 0;
        for (const auto model : AssetLoader::Instance().GetModels()) {
            if (model != nullptr) {
                ++loadedModelCount;
            }
        }

        int materialCount = static_cast<int>(materials.size());
        int sphereCount = static_cast<int>(spheres.size());
        int triangleCount = static_cast<int>(triangles.size());
        int boxCount = static_cast<int>(boxes.size());
        int modelCount = loadedModelCount;

        os.write(reinterpret_cast<const char*>(&materialCount), sizeof(materialCount));
        os.write(reinterpret_cast<const char*>(&sphereCount), sizeof(sphereCount));
        os.write(reinterpret_cast<const char*>(&triangleCount), sizeof(triangleCount));
        os.write(reinterpret_cast<const char*>(&boxCount), sizeof(boxCount));
        os.write(reinterpret_cast<const char*>(&modelCount), sizeof(modelCount));

        for (const auto &material : materials) {
            os.write(reinterpret_cast<const char*>(material.textures[TextureIndex::Albedo]->GetData()), sizeof(Math::Vector3f));
            os.write(reinterpret_cast<const char*>(material.textures[TextureIndex::Metallic]->GetData()), sizeof(float));
            os.write(reinterpret_cast<const char*>(material.textures[TextureIndex::Specular]->GetData()), sizeof(float));
            os.write(reinterpret_cast<const char*>(material.textures[TextureIndex::Roughness]->GetData()), sizeof(float));
            os.write(reinterpret_cast<const char*>(&material.emissionPower), sizeof(material.emissionPower));
            os.write(reinterpret_cast<const char*>(&material.index), sizeof(material.index));
        }

        for (const auto &sphere : spheres) {
            os.write(reinterpret_cast<const char*>(&sphere.center), sizeof(sphere.center));
            os.write(reinterpret_cast<const char*>(&sphere.radius), sizeof(sphere.radius));
            os.write(reinterpret_cast<const char*>(&sphere.material->index), sizeof(sphere.material->index));
        }

        for (const auto &triangle : triangles) {
            os.write(reinterpret_cast<const char*>(&triangle.vertices), sizeof(triangle.vertices));
            os.write(reinterpret_cast<const char*>(&triangle.normal), sizeof(triangle.normal));
            os.write(reinterpret_cast<const char*>(&triangle.material->index), sizeof(triangle.material->index));
        }

        for (const auto &box : boxes) {
            os.write(reinterpret_cast<const char*>(&box.min), sizeof(box.min));
            os.write(reinterpret_cast<const char*>(&box.max), sizeof(box.max));
            os.write(reinterpret_cast<const char*>(&box.material->index), sizeof(box.material->index));
        }

        for (const auto model : AssetLoader::Instance().GetModels()) {
            if (model == nullptr) {
                continue;
            }

            auto pathToFile = model->GetPathToFile().string();
            int pathToFileLength = static_cast<int>(pathToFile.length());
            
            auto materialDirectory = model->GetMaterialDirectory().string();
            int materialDirectoryLength = static_cast<int>(materialDirectory.length());
            
            os.write(reinterpret_cast<const char*>(&pathToFileLength), sizeof(pathToFileLength));
            os.write(pathToFile.data(), pathToFileLength);

            os.write(reinterpret_cast<const char*>(&materialDirectoryLength), sizeof(materialDirectoryLength));
            os.write(materialDirectory.data(), materialDirectoryLength);
        }

        auto position = camera.GetPosition();
        auto target = camera.GetTarget();
        auto verticalFovInDegrees = camera.GetVerticalFovInDegrees();
        auto up = camera.GetUp();

        os.write(reinterpret_cast<const char*>(&position), sizeof(position));
        os.write(reinterpret_cast<const char*>(&target), sizeof(target));
        os.write(reinterpret_cast<const char*>(&verticalFovInDegrees), sizeof(verticalFovInDegrees));
        os.write(reinterpret_cast<const char*>(&up), sizeof(up));
    }

    void TryDeserialize(std::istream &is) {
        int materialCount;
        int sphereCount;
        int triangleCount;
        int boxCount;
        int modelCount;

        is.read(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));
        is.read(reinterpret_cast<char*>(&sphereCount), sizeof(sphereCount));
        is.read(reinterpret_cast<char*>(&triangleCount), sizeof(triangleCount));
        is.read(reinterpret_cast<char*>(&boxCount), sizeof(boxCount));
        is.read(reinterpret_cast<char*>(&modelCount), sizeof(modelCount));

        materials.clear();
        materials.resize(materialCount);
        for (auto &material : materials) {
            for (int i = TextureIndex::Albedo; i <= TextureIndex::Bump; ++i) {
                if (material.textures[i] == nullptr) {
                    material.textures[i] = new Texture();
                }
            }

            is.read(reinterpret_cast<char*>(material.textures[TextureIndex::Albedo]->GetData()), sizeof(Math::Vector3f));
            is.read(reinterpret_cast<char*>(material.textures[TextureIndex::Metallic]->GetData()), sizeof(float));
            is.read(reinterpret_cast<char*>(material.textures[TextureIndex::Specular]->GetData()), sizeof(float));
            is.read(reinterpret_cast<char*>(material.textures[TextureIndex::Roughness]->GetData()), sizeof(float));
            is.read(reinterpret_cast<char*>(&material.emissionPower), sizeof(material.emissionPower));
            is.read(reinterpret_cast<char*>(&material.index), sizeof(material.index));
        }
        materials.shrink_to_fit();

        spheres.clear();
        spheres.reserve(sphereCount);
        while (sphereCount--) {
            Math::Vector3f center;
            float radius;
            int materialIndex;
            is.read(reinterpret_cast<char*>(&center), sizeof(center));
            is.read(reinterpret_cast<char*>(&radius), sizeof(radius));
            is.read(reinterpret_cast<char*>(&materialIndex), sizeof(materialIndex));            
            
            spheres.emplace_back(center, radius, &materials[materialIndex]);
        }
        spheres.shrink_to_fit();

        triangles.clear();
        triangles.reserve(triangleCount);
        while (triangleCount--) {
            Math::Vector3f vertices[3];
            Math::Vector3f normal;
            int materialIndex;
            is.read(reinterpret_cast<char*>(&vertices), sizeof(vertices));
            is.read(reinterpret_cast<char*>(&normal), sizeof(normal));
            is.read(reinterpret_cast<char*>(&materialIndex), sizeof(materialIndex));

            triangles.emplace_back(std::array<Math::Vector3f, 3>{vertices[0], vertices[1], vertices[2]}, normal, &materials[materialIndex]);
        }
        triangles.shrink_to_fit();

        boxes.clear();
        boxes.reserve(boxCount);
        while (boxCount--) {
            Math::Vector3f min, max;
            int materialIndex;
            is.read(reinterpret_cast<char*>(&min), sizeof(min));
            is.read(reinterpret_cast<char*>(&max), sizeof(max));
            is.read(reinterpret_cast<char*>(&materialIndex), sizeof(materialIndex));

            boxes.emplace_back(min, max, &materials[materialIndex]);
        }
        boxes.shrink_to_fit();

        for (const auto model : modelInstances) {
            delete model;
        }

        modelInstances.clear();
        modelInstances.reserve(modelCount);
        while (modelCount--) {
            int pathToFileLength;
            is.read(reinterpret_cast<char*>(&pathToFileLength), sizeof(pathToFileLength));

            std::vector<char> buffer(pathToFileLength);
            is.read(buffer.data(), pathToFileLength);

            std::string pathToFile(buffer.data(), pathToFileLength);
            
            int materialDirectoryLength;
            is.read(reinterpret_cast<char*>(&materialDirectoryLength), sizeof(materialDirectoryLength));

            buffer.resize(materialDirectoryLength);
            is.read(buffer.data(), materialDirectoryLength);
            
            std::string materialDirectory(buffer.data(), materialDirectoryLength);

            ModelInstance *modelInstance;
            AssetLoader::Result result;
            double loadTime = Timer::MeasureInMillis([&](){
                auto [instance, res] = AssetLoader::Instance().LoadOBJ(pathToFile, materialDirectory);
                modelInstance = instance;
                result = res;
            });

            if (result.IsFailure()) {
                printf("Failed to load model %s, error:\n%s", pathToFile.c_str(), result.error.c_str());
                continue;
            }

            printf("Time to load model %s is %fms\n", pathToFile.c_str(), loadTime);

            modelInstances.push_back(modelInstance);
        }
        modelInstances.shrink_to_fit();

        is.read(reinterpret_cast<char*>(&camera.Position()), sizeof(camera.Position()));
        is.read(reinterpret_cast<char*>(&camera.Target()), sizeof(camera.Target()));
        is.read(reinterpret_cast<char*>(&camera.VerticalFovInDegrees()), sizeof(camera.VerticalFovInDegrees()));
        is.read(reinterpret_cast<char*>(&camera.Up()), sizeof(camera.Up()));
    }
};

#endif