#include <filesystem>
#define TINYOBJLOADER_IMPLEMENTATION
#include "AssetLoader.h"

#include "../../stb-master/stb_image.h"

AssetLoader::AssetLoader() noexcept {
    m_LoadingProperties.generateSmoothNormals = true;
    m_LoadingProperties.surfaceAreaWeighting = true;
}

AssetLoader::~AssetLoader() noexcept {
    for (int modelIndex = 0; modelIndex < static_cast<int>(m_Models.size()); ++modelIndex) {
        if (m_InstanceCount[modelIndex] > 0) {
            UnloadModel(modelIndex);
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

    m_TextureAbsolutePaths.emplace_back();

    std::vector<Material> pbrMaterials;
    pbrMaterials.reserve(materials.size());
    for (const auto &material : materials) {
        pbrMaterials.push_back(ProcessMaterial(material, static_cast<int>(pbrMaterials.size()), materialDirectory));
    }

    std::vector<Mesh*> meshes;
    meshes.reserve(shapes.size());

    int totalFaceCount = 0;
    for (const auto &shape : shapes) {
        const auto &mesh = shape.mesh;

        int faceCount = static_cast<int>(mesh.num_face_vertices.size());
        totalFaceCount += faceCount;

        meshes.push_back(ProcessMesh(attrib, mesh));
    }

    Model *model = new Model(pathToFile, materialDirectory, std::move(meshes), std::move(pbrMaterials), totalFaceCount);

    int modelIndex = static_cast<int>(m_Models.size());

    m_InstanceCount.push_back(0);
    ModelInstance *modelInstance = new ModelInstance(
        model->GetBVH(),
        [this, modelIndex]() {
            IncreaseInstanceCount(modelIndex);
        },
        [this, modelIndex]() {
            DecreaseInstanceCount(modelIndex);
        }
    );

    m_Models.push_back(model);

    return {modelInstance, result};
}

Material AssetLoader::ProcessMaterial(const tinyobj::material_t &material, int index, const std::filesystem::path &materialDirectory) noexcept {
    Material resultMaterial;
    resultMaterial.textures[TextureIndex::Albedo] = new Texture(Math::Vector3f(material.diffuse[0], material.diffuse[1], material.diffuse[2]));
    resultMaterial.textures[TextureIndex::Metallic] = new Texture(Math::Vector3f(Math::Max(material.specular[0], Math::Max(material.specular[1], material.specular[2]))));
    resultMaterial.textures[TextureIndex::Roughness] = new Texture(Math::Vector3f(0.5f));
    resultMaterial.textures[TextureIndex::Specular] = new Texture(Math::Vector3f(0.f));
    resultMaterial.index = index;

    std::array<std::string, 5> textureNames = {
        material.diffuse_texname,
        material.reflection_texname,
        material.specular_texname,
        material.specular_highlight_texname,
        material.bump_texname
    };

    FixTextureNames(textureNames);

    for (int i = 0; i < static_cast<int>(textureNames.size()); ++i) {
        const auto &textureName = textureNames[i];

        if (textureName.empty()) {
            continue;
        }

        std::filesystem::path pathToTexture = materialDirectory / textureName;
        std::string absolutePathToTexture = std::filesystem::absolute(pathToTexture).generic_string();

        Texture *texture = nullptr;

        auto it = m_Textures.find(absolutePathToTexture);
        if (it != m_Textures.cend()) {
            texture = it->second;
        } else {
            texture = LoadTexture(pathToTexture);
            m_TextureAbsolutePaths.back().push_back(absolutePathToTexture);
        }

        if (resultMaterial.textures[i] != nullptr) {
            delete resultMaterial.textures[i];
        }

        resultMaterial.textures[i] = texture;
    }

    // printf("Transparent: %f, refraction index: %f\n", material.dissolve, material.ior);

    // pbrMaterial.transparency = material.dissolve;
    // pbrMaterial.refractionIndex = material.ior;

    return resultMaterial;
}

void AssetLoader::FixTextureNames(std::span<std::string> names) noexcept {
    for (auto &name : names) {
        std::replace(name.begin(), name.end(), '\\', '/');
    }
}

Texture* AssetLoader::LoadTexture(const std::filesystem::path &pathToTexture) noexcept {
    const int DESIRED_CHANNELS = 3;
    int width, height, channels;
    unsigned char *textureDataInBytes = stbi_load(pathToTexture.generic_string().c_str(), &width, &height, &channels, DESIRED_CHANNELS);

    printf("%s: %dx%d with %d channels\n", pathToTexture.generic_string().c_str(), width, height, channels);

    Texture *texture = new Texture(textureDataInBytes, width, height, DESIRED_CHANNELS);

    stbi_image_free(textureDataInBytes);

    std::string absolutePathToTexture = std::filesystem::absolute(pathToTexture).string();

    return m_Textures.insert({absolutePathToTexture, texture}).first->second;
}

Mesh* AssetLoader::ProcessMesh(const tinyobj::attrib_t &attrib, const tinyobj::mesh_t &mesh) noexcept {
    const int VERTICES_PER_FACE = 3;

    int faceCount = static_cast<int>(mesh.num_face_vertices.size());
    std::unordered_map<Mesh::Vertex, int> uniqueVertices;

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
            auto vertex = ProcessVertex(attrib, index);

            int i = static_cast<int>(uniqueVertices.size());
            auto [it, inserted] = uniqueVertices.insert({vertex, i});
            indices.push_back(inserted ? i : it->second);
        }

        materialIndices.push_back(mesh.material_ids[faceIndex]);

        offset += VERTICES_PER_FACE;
    }

    vertices.reserve(uniqueVertices.size());
    for (const auto &[vertex, index] : uniqueVertices) {
        vertices.push_back(vertex);
    }

    std::sort(vertices.begin(), vertices.end(), [&](const auto &a, const auto &b) {
        return uniqueVertices.at(a) < uniqueVertices.at(b);
    });

    if (attrib.normals.empty()) {
        GenerateNormals(vertices, indices, faceCount);
    }

    GenerateTangents(vertices, indices, faceCount);

    vertices.shrink_to_fit();
    indices.shrink_to_fit();
    materialIndices.shrink_to_fit();

    return new Mesh(std::move(vertices), std::move(indices), std::move(materialIndices));
}

Mesh::Vertex AssetLoader::ProcessVertex(const tinyobj::attrib_t &attrib, const tinyobj::index_t &index) noexcept {
    Mesh::Vertex vertex;

    if (!attrib.vertices.empty()) {
        vertex.position = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2]
        };
    }

    if (!attrib.normals.empty()) {
        vertex.normal = {
            attrib.normals[3 * index.normal_index + 0],
            attrib.normals[3 * index.normal_index + 1],
            attrib.normals[3 * index.normal_index + 2]
        };
    }

    if (!attrib.texcoords.empty()) {
        vertex.texcoord = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            1.f - attrib.texcoords[2 * index.texcoord_index + 1]
        };
    }

    return vertex;
}

void AssetLoader::GenerateNormals(std::vector<Mesh::Vertex> &vertices, const std::vector<int> &indices, int faceCount) noexcept {
    for (int f = 0; f < faceCount; ++f) {
        auto &v0 = vertices[indices[3 * f + 0]];
        auto &v1 = vertices[indices[3 * f + 1]];
        auto &v2 = vertices[indices[3 * f + 2]];

        const auto &p0 = v0.position;
        const auto &p1 = v1.position;
        const auto &p2 = v2.position;

        auto normal = Math::Cross(p1 - p0, p2 - p0);

        if (!m_LoadingProperties.generateSmoothNormals) {
            break;
        }

        if (!m_LoadingProperties.surfaceAreaWeighting) {
            normal = Math::Normalize(normal);
        }

        float a0 = Math::Angle(p1 - p0, p2 - p0);
        float a1 = Math::Angle(p2 - p1, p0 - p1);
        float a2 = Math::Angle(p0 - p2, p1 - p2);

        v0.normal += normal * a0;
        v1.normal += normal * a1;
        v2.normal += normal * a2;
    }

    for (auto &vertex : vertices) {
        vertex.normal = Math::Normalize(vertex.normal);
    }
}


void AssetLoader::GenerateTangents(std::vector<Mesh::Vertex> &vertices, const std::vector<int> &indices, int faceCount) noexcept {
    for (int f = 0; f < faceCount; ++f) {
        auto &v0 = vertices[indices[3 * f + 0]];
        auto &v1 = vertices[indices[3 * f + 1]];
        auto &v2 = vertices[indices[3 * f + 2]];

        const auto &p0 = v0.position;
        const auto &p1 = v1.position;
        const auto &p2 = v2.position;

        const auto &t0 = v0.texcoord;
        const auto &t1 = v1.texcoord;
        const auto &t2 = v2.texcoord;

        auto dp1 = p1 - p0;
        auto dp2 = p2 - p0;
        auto dt1 = t1 - t0;
        auto dt2 = t2 - t0;

        float inverseDeterminant = 1.f / (dt1.x * dt2.y - dt2.x * dt1.y);

        Math::Vector3f tangent;
        tangent.x = inverseDeterminant * (dt2.y * dp1.x - dt1.y * dp2.x);
        tangent.y = inverseDeterminant * (dt2.y * dp1.y - dt1.y * dp2.y);
        tangent.z = inverseDeterminant * (dt2.y * dp1.z - dt1.y * dp2.z);

        v0.tangent += tangent;
        v1.tangent += tangent;
        v2.tangent += tangent;
    }

    for (auto &vertex : vertices) {
        vertex.tangent = Math::Normalize(vertex.tangent - Math::Dot(vertex.tangent, vertex.normal) * vertex.normal);
    }
}

void AssetLoader::IncreaseInstanceCount(int modelIndex) noexcept {
    ++m_InstanceCount[modelIndex];
}

void AssetLoader::DecreaseInstanceCount(int modelIndex) noexcept {
    if (--m_InstanceCount[modelIndex] == 0) {
        UnloadModel(modelIndex);
    }
}

void AssetLoader::UnloadModel(int modelIndex) noexcept {
    if (modelIndex >= static_cast<int>(m_Models.size())) {
        return;
    }

    if (m_Models[modelIndex] != nullptr) {
        delete m_Models[modelIndex];
        m_Models[modelIndex] = nullptr;
    }

    for (const auto &absolutePathToTexture : m_TextureAbsolutePaths[modelIndex]) {
        printf("Unloading texture: %s\n", absolutePathToTexture.c_str());

        auto it = m_Textures.find(absolutePathToTexture);
        if (it == m_Textures.cend()) {
            continue;
        }

        if (it->second != nullptr) {
            delete it->second;
        }

        m_Textures.erase(it);
    }
}
