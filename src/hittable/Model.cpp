#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../tinyobjloader/tiny_obj_loader.h"

#include "../../stb-master/stb_image.h"

Model::Model(const std::filesystem::path &pathToFile, const std::filesystem::path &materialDirectory, std::vector<Mesh> &&meshes, std::vector<Material> &&materials, std::vector<std::vector<uint32_t>> &&textures) noexcept : 
    m_PathToFile(pathToFile), m_MaterialDirectory(materialDirectory), m_Meshes(std::move(meshes)), m_Materials(std::move(materials)), m_Textures(std::move(textures)) {
    int totalVertexCount = 0;
    for (const auto &mesh : m_Meshes) {
        Math::Vector3f sum(0.f);
        auto vertices = mesh.GetVertices();
        for (const auto &vertex : vertices) {
            sum += vertex.position;
        }

        totalVertexCount += static_cast<int>(vertices.size());
        centroid += sum;
    }

    centroid /= static_cast<float>(totalVertexCount);

    int totalFaceCount = 0;
    for (const auto &mesh : m_Meshes) {
        totalFaceCount += static_cast<int>(mesh.GetIndices().size() / 3);
    }

    m_Polygons.reserve(totalFaceCount);
    for (auto &mesh : m_Meshes) {
        auto vertices = mesh.GetVertices();
        auto indices = mesh.GetIndices();
        auto materialIndices = mesh.GetMaterialIndices();

        int faceCount = static_cast<int>(indices.size() / 3);
        for (int f = 0; f < faceCount; ++f) {
            auto &material = m_Materials[materialIndices[f]];
            m_Polygons.emplace_back(&mesh, &material, f);

            if (material.emissionPower > 0.f) {
                m_LightSources.emplace_back(&m_Polygons.back());
            }
        }
    }

    std::vector<HittableObjectPtr> objects;
    for (auto &polygon : m_Polygons) {
        objects.push_back(&polygon);
    }

    m_Root = BVHNode::MakeHierarchySAH(objects, 0, static_cast<int>(objects.size()));
}

Model::LoadResult Model::LoadOBJ(const std::filesystem::path &pathToFile, const std::filesystem::path &materialDirectory) noexcept {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning;
    std::string error;
    tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, pathToFile.string().c_str(), materialDirectory.string().c_str());
    
    if (!error.empty()) {
        return LoadResult::Error(error);
    }

    std::vector<std::vector<uint32_t>> textures;

    std::vector<Material> pbrMaterials;
    pbrMaterials.reserve(materials.size());
    for (const auto &material : materials) {
        Material pbrMaterial;
        // pbrMaterial.emissionPower = Math::Max(material.ambient[0], Math::Max(material.ambient[1], material.ambient[2]));
        pbrMaterial.albedo = Texture(Math::Vector3f(material.diffuse[0], material.diffuse[1], material.diffuse[2]));
        pbrMaterial.metallic = Texture(Math::Vector3f(Math::Max(material.specular[0], Math::Max(material.specular[1], material.specular[2]))));
        pbrMaterial.roughness = Texture(Math::Vector3f(0.5f));
        pbrMaterial.specular = Texture(Math::Vector3f(0.f));
        pbrMaterial.index = static_cast<int>(pbrMaterials.size());

        auto textureNames = {material.diffuse_texname, material.specular_highlight_texname, material.reflection_texname, material.specular_texname, material.bump_texname};

        for (int i = 0; i < (int)textureNames.size(); ++i) {
            const auto &textureName = *(textureNames.begin() + i);

            if (textureName.empty()) {
                continue;
            }

            std::filesystem::path pathToTexture = materialDirectory / std::filesystem::path(textureName);

            const int desiredChannels = 3;
            int width, height, channels;
            unsigned char *textureData = stbi_load(pathToTexture.string().c_str(), &width, &height, &channels, desiredChannels);

            printf("%s: %dx%d with %d channels\n", pathToTexture.string().c_str(), width, height, channels);

            Texture texture(std::span<const unsigned char>{textureData, std::dynamic_extent}, width, height, desiredChannels);

            stbi_image_free(textureData);

            switch (i)
            {
            case 0:
                pbrMaterial.albedo = texture;
                break;
            case 1:
                pbrMaterial.roughness = texture;
                break;
            case 2:
                pbrMaterial.metallic = texture;
                break;
            case 3:
                pbrMaterial.specular = texture;
            case 4:
                pbrMaterial.bump = texture;
                break;
            default:
                break;
            }
        }

        pbrMaterials.push_back(pbrMaterial);
    }

    std::vector<Mesh> meshes;
    meshes.reserve(shapes.size());

    for (const auto &shape : shapes) {
        const auto &mesh = shape.mesh;

        std::vector<Mesh::Vertex> vertices;
        int faceCount = static_cast<int>(mesh.num_face_vertices.size());
        vertices.reserve(faceCount * c_VerticesPerFace);
    
        std::vector<int> indices;
        indices.reserve(faceCount * c_VerticesPerFace);

        std::vector<int> materialIndices;
        materialIndices.reserve(faceCount);

        int offset = 0;
        for (int faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
            for (int vertexIndex = 0; vertexIndex < c_VerticesPerFace; ++vertexIndex) {
                auto index = mesh.indices[offset + vertexIndex];

                float x = 0.f, y = 0.f, z = 0.f;
                if (!attrib.vertices.empty()) {
                    x = attrib.vertices[3 * index.vertex_index + 0];
                    y = attrib.vertices[3 * index.vertex_index + 1];
                    z = attrib.vertices[3 * index.vertex_index + 2];
                }

                float nx = 0.f, ny = 0.f, nz = 0.f;
                if (!attrib.normals.empty()) {
                    nx = attrib.normals[3 * index.normal_index + 0];
                    ny = attrib.normals[3 * index.normal_index + 1];
                    nz = attrib.normals[3 * index.normal_index + 2];
                }

                float u = 0.f, v = 0.f;
                if (!attrib.texcoords.empty()) {
                    u = attrib.texcoords[2 * index.texcoord_index + 0];
                    v = attrib.texcoords[2 * index.texcoord_index + 1];
                }

                Mesh::Vertex vertex;
                vertex.position = {x, y, z};
                vertex.normal = {nx, ny, nz};
                vertex.texcoord = {u, v};

                int i = static_cast<int>(vertices.size());
                vertices.push_back(vertex);
                indices.push_back(i);
            }

            materialIndices.push_back(mesh.material_ids[faceIndex]);

            offset += c_VerticesPerFace;
        }

        if (attrib.normals.empty()) {
            for (int f = 0; f < faceCount; ++f) {
                auto &v0 = vertices[indices[3 * f + 0]];
                auto &v1 = vertices[indices[3 * f + 1]];
                auto &v2 = vertices[indices[3 * f + 2]];

                Math::Vector3f faceNormal = Math::Normalize(Math::Cross(v1.position - v0.position, v2.position - v0.position));
                v0.normal = faceNormal;
                v1.normal = faceNormal;
                v2.normal = faceNormal;
            }
        }

        vertices.shrink_to_fit();
        meshes.emplace_back(std::move(vertices), std::move(indices), std::move(materialIndices));
    }

    LoadResult result;
    result.model = new Model(pathToFile, materialDirectory, std::move(meshes), std::move(pbrMaterials), std::move(textures));
    result.warning = warning;

    return result;
}

bool Model::Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept {
    return m_Root->Hit(ray, tMin, tMax, payload);
}

Math::Vector3f Model::GetCentroid() const noexcept {
    return centroid;
}

AABB Model::GetBoundingBox() const noexcept {
    return m_Root->aabb;
}