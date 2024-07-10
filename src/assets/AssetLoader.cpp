#include "AssetLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../tinyobjloader/tiny_obj_loader.h"

#include "../../stb-master/stb_image.h"

AssetLoader::~AssetLoader() noexcept {
    for (auto &model : m_Models) {
        if (model != nullptr) {
            delete model;
            model = nullptr;
        }
    }
}

std::pair<ModelInstance*, AssetLoader::Result> AssetLoader::LoadOBJ(const std::filesystem::path &pathToFile, const std::filesystem::path &materialDirectory) noexcept {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    Result result;
    tinyobj::LoadObj(&attrib, &shapes, &materials, &result.warning, &result.error, pathToFile.string().c_str(), materialDirectory.string().c_str());
    
    if (result.IsFailure()) {
        return {nullptr, result};
    }

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
        
        auto textureNames = {
            material.diffuse_texname,
            material.specular_highlight_texname,
            material.reflection_texname,
            material.specular_texname,
            material.bump_texname
        };

        for (int i = 0; i < static_cast<int>(textureNames.size()); ++i) {
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
                break;
            case 4:
                pbrMaterial.bump = texture;
                break;
            default:
                break;
            }
        }
        
        printf("Transparent: %f, refraction index: %f\n", material.dissolve, material.ior);

        pbrMaterial.transparency = material.dissolve;
        pbrMaterial.refractionIndex = material.ior;

        pbrMaterials.push_back(pbrMaterial);
    }

    std::vector<Mesh> meshes;
    meshes.reserve(shapes.size());

    int totalFaceCount = 0;

    const int VERTICES_PER_FACE = 3;

    for (const auto &shape : shapes) {
        const auto &mesh = shape.mesh;

        int faceCount = static_cast<int>(mesh.num_face_vertices.size());
        totalFaceCount += faceCount;
        
        std::vector<Mesh::Vertex> vertices;
        vertices.reserve(faceCount * VERTICES_PER_FACE);
    
        std::vector<int> indices;
        indices.reserve(faceCount * VERTICES_PER_FACE);

        std::vector<int> materialIndices;
        materialIndices.reserve(faceCount);

        int offset = 0;
        for (int faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
            for (int vertexIndex = 0; vertexIndex < VERTICES_PER_FACE; ++vertexIndex) {
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

            offset += VERTICES_PER_FACE;
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

    Model *model = new Model(pathToFile, materialDirectory, std::move(meshes), std::move(pbrMaterials), totalFaceCount);

    m_InstanceCount.push_back(0);
    ModelInstance *modelInstance = new ModelInstance(static_cast<int>(m_Models.size()), model->GetBVH());

    m_Models.push_back(model);

    return {modelInstance, result};
}

void AssetLoader::IncreaseInstanceCount(int modelIndex) noexcept {
    ++m_InstanceCount[modelIndex];
}

void AssetLoader::DecreaseInstanceCount(int modelIndex) noexcept{
    if (--m_InstanceCount[modelIndex] == 0) {
        delete m_Models[modelIndex];
        m_Models[modelIndex] = nullptr;
    }
}