#ifndef _SCENE_H
#define _SCENE_H

#include "hittable/Sphere.h"
#include "hittable/Triangle.h"
#include "hittable/Box.h"
#include "Material.h"
#include "Camera.h"

#include <vector>
#include <fstream>
#include <array>
#include <exception>
#include <optional>

struct Scene {
    std::vector<Shapes::Sphere> spheres;
    std::vector<Shapes::Triangle> triangles;
    std::vector<Shapes::Box> boxes;
    std::vector<Material> materials;
    Camera camera;

    std::optional<std::string> Serialize(std::ostream &os) const noexcept {
        os.exceptions(std::ios::badbit | std::ios::failbit);

        try {
            TrySerialize(os);
        } catch (std::exception &e) {
            return e.what();
        }

        return {};
    }

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
        int sphereCount = static_cast<int>(spheres.size());
        int triangleCount = static_cast<int>(triangles.size());
        int boxCount = static_cast<int>(boxes.size());
        int modelCount = 0;
        int materialCount = static_cast<int>(materials.size());

        os.write(reinterpret_cast<const char*>(&sphereCount), sizeof(sphereCount));
        os.write(reinterpret_cast<const char*>(&triangleCount), sizeof(triangleCount));
        os.write(reinterpret_cast<const char*>(&boxCount), sizeof(boxCount));
        os.write(reinterpret_cast<const char*>(&modelCount), sizeof(modelCount));
        os.write(reinterpret_cast<const char*>(&materialCount), sizeof(materialCount));

        for (const auto &sphere : spheres) {
            os.write(reinterpret_cast<const char*>(&sphere.center), sizeof(sphere.center));
            os.write(reinterpret_cast<const char*>(&sphere.radius), sizeof(sphere.radius));
            os.write(reinterpret_cast<const char*>(&sphere.materialIndex), sizeof(sphere.materialIndex));
        }

        for (const auto &triangle : triangles) {
            os.write(reinterpret_cast<const char*>(&triangle.vertices), sizeof(triangle.vertices));
            os.write(reinterpret_cast<const char*>(&triangle.normal), sizeof(triangle.normal));
            os.write(reinterpret_cast<const char*>(&triangle.materialIndex), sizeof(triangle.materialIndex));
        }

        for (const auto &box : boxes) {
            os.write(reinterpret_cast<const char*>(&box.min), sizeof(box.min));
            os.write(reinterpret_cast<const char*>(&box.max), sizeof(box.max));
            os.write(reinterpret_cast<const char*>(&box.materialIndex), sizeof(box.materialIndex));
        }

        // todo! loop for models here

        for (const auto &material : materials) {
            os.write(reinterpret_cast<const char*>(&material), sizeof(material));
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
        int sphereCount;
        int triangleCount;
        int boxCount;
        int modelCount;
        int materialCount;

        is.read(reinterpret_cast<char*>(&sphereCount), sizeof(sphereCount));
        is.read(reinterpret_cast<char*>(&triangleCount), sizeof(triangleCount));
        is.read(reinterpret_cast<char*>(&boxCount), sizeof(boxCount));
        is.read(reinterpret_cast<char*>(&modelCount), sizeof(modelCount));
        is.read(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));

        spheres.clear();
        spheres.shrink_to_fit();
        spheres.reserve(sphereCount);
        while (sphereCount--) {
            Math::Vector3f center;
            float radius;
            int materialIndex;
            is.read(reinterpret_cast<char*>(&center), sizeof(center));
            is.read(reinterpret_cast<char*>(&radius), sizeof(radius));
            is.read(reinterpret_cast<char*>(&materialIndex), sizeof(materialIndex));            
            
            spheres.emplace_back(center, radius, materialIndex);
        }

        triangles.clear();
        triangles.shrink_to_fit();
        triangles.reserve(triangleCount);
        while (triangleCount--) {
            Math::Vector3f vertices[3];
            Math::Vector3f normal;
            int materialIndex;
            is.read(reinterpret_cast<char*>(&vertices), sizeof(vertices));
            is.read(reinterpret_cast<char*>(&normal), sizeof(normal));
            is.read(reinterpret_cast<char*>(&materialIndex), sizeof(materialIndex));

            triangles.emplace_back(std::array<Math::Vector3f, 3>{vertices[0], vertices[1], vertices[2]}, normal, materialIndex);
        }

        boxes.clear();
        boxes.shrink_to_fit();
        boxes.reserve(boxCount);

        while (boxCount--) {
            Math::Vector3f min, max;
            int materialIndex;
            is.read(reinterpret_cast<char*>(&min), sizeof(min));
            is.read(reinterpret_cast<char*>(&max), sizeof(max));
            is.read(reinterpret_cast<char*>(&materialIndex), sizeof(materialIndex));

            boxes.emplace_back(min, max, materialIndex);
        }

        while (modelCount--) {

        }

        materials.clear();
        materials.shrink_to_fit();
        materials.resize(materialCount);
        for (auto &material : materials) {
            is.read(reinterpret_cast<char*>(&material), sizeof(material));
        }

        is.read(reinterpret_cast<char*>(&camera.Position()), sizeof(camera.Position()));
        is.read(reinterpret_cast<char*>(&camera.Target()), sizeof(camera.Target()));
        is.read(reinterpret_cast<char*>(&camera.VerticalFovInDegrees()), sizeof(camera.VerticalFovInDegrees()));
        is.read(reinterpret_cast<char*>(&camera.Up()), sizeof(camera.Up()));

        camera.ComputeRayDirections();
    }
};

#endif