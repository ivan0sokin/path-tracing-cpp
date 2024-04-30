#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../tinyobjloader/tiny_obj_loader.h"

#include <unordered_map>

Model::Model(std::vector<Mesh> &&meshes, std::vector<Material> &&materials) noexcept : m_Meshes(std::move(meshes)), m_Materials(std::move(materials)) {
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
            m_Polygons.emplace_back(&mesh, &m_Materials[materialIndices[f]], f);
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

    std::vector<Material> pbrMaterials;
    pbrMaterials.reserve(materials.size());
    for (const auto &material : materials) {
        Material pbrMaterial;
        pbrMaterial.emissionPower = Math::Max(material.ambient[0], Math::Max(material.ambient[1], material.ambient[2]));
        pbrMaterial.albedo = Math::Vector3f(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        pbrMaterial.metallic = Math::Max(material.specular[0], Math::Max(material.specular[1], material.specular[2]));
        pbrMaterial.roughness = 0.5f;
        pbrMaterial.specular = 0.f;
        pbrMaterial.index = static_cast<int>(pbrMaterials.size());

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

        std::unordered_map<Mesh::Vertex, int> vertexToIndex;

        int offset = 0;
        for (int f = 0; f < faceCount; ++f) {
            for (int v = 0; v < c_VerticesPerFace; ++v) {
                auto index = mesh.indices[offset + v];
                float x = attrib.vertices[3 * index.vertex_index + 0];
                float y = attrib.vertices[3 * index.vertex_index + 1];
                float z = attrib.vertices[3 * index.vertex_index + 2];


                float nx = 0.f, ny = 0.f, nz = 0.f;
                if (!attrib.normals.empty()) {
                    nx = attrib.normals[3 * index.normal_index + 0];
                    ny = attrib.normals[3 * index.normal_index + 1];
                    nz = attrib.normals[3 * index.normal_index + 2];
                }

                Mesh::Vertex vertex;
                vertex.position = Math::Vector3f(x, y, z);
                vertex.normal = Math::Vector3f(nx, ny, nz);

                auto it = vertexToIndex.find(vertex);
                if (it == vertexToIndex.cend()) {
                    int i = static_cast<int>(vertices.size());
                    vertexToIndex.emplace(vertex, i);
                    vertices.push_back(vertex);
                    
                    indices.push_back(i);
                } else {
                    indices.push_back(it->second);
                }

                // tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                // tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                // Optional: vertex colors
                // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
            }

            materialIndices.push_back(mesh.material_ids[f]);

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
    result.model = new Model(std::move(meshes), std::move(pbrMaterials));
    result.warning = warning;

    return result;
}

void Model::Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept {
    m_Root->Hit(ray, tMin, tMax, payload);
}

Math::Vector3f Model::GetCentroid() const noexcept {
    return centroid;
}

AABB Model::GetBoundingBox() const noexcept {
    return m_Root->aabb;
}