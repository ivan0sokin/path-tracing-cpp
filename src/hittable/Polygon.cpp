#include "../assets/Model.h"
#include "Polygon.h"

Shapes::Triangle Polygon::MakeTriangle() const noexcept {
    const Mesh &mesh = m_Model->GetMeshes()[m_MeshIndex];
    
    auto vertices = mesh.GetVertices();
    auto indices = mesh.GetIndices();

    auto materials = m_Model->GetMaterials();
    auto materialIndices = mesh.GetMaterialIndices();
        
    return Shapes::Triangle(vertices[indices[3 * m_FaceIndex + 0]].position, vertices[indices[3 * m_FaceIndex + 1]].position, vertices[indices[3 * m_FaceIndex + 2]].position, &materials[materialIndices[m_FaceIndex]]);
}

bool Polygon::Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept {
    if (!MakeTriangle().Hit(ray, tMin, tMax, payload)) {
        return false;
    }

    const Mesh &mesh = m_Model->GetMeshes()[m_MeshIndex];

    auto vertices = mesh.GetVertices();
    auto indices = mesh.GetIndices();

    auto v0 = vertices[indices[3 * m_FaceIndex + 0]].position;
    auto v1 = vertices[indices[3 * m_FaceIndex + 1]].position;
    auto v2 = vertices[indices[3 * m_FaceIndex + 2]].position;

    auto n0 = vertices[indices[3 * m_FaceIndex + 0]].normal;
    auto n1 = vertices[indices[3 * m_FaceIndex + 1]].normal;
    auto n2 = vertices[indices[3 * m_FaceIndex + 2]].normal;

    n0 = Math::Dot(ray.direction, n0) > Math::Constants::Epsilon<float> ? -n0 : n0;
    n1 = Math::Dot(ray.direction, n1) > Math::Constants::Epsilon<float> ? -n1 : n1;
    n2 = Math::Dot(ray.direction, n2) > Math::Constants::Epsilon<float> ? -n2 : n2;

    auto t0 = vertices[indices[3 * m_FaceIndex + 0]].texcoord;
    auto t1 = vertices[indices[3 * m_FaceIndex + 1]].texcoord;
    auto t2 = vertices[indices[3 * m_FaceIndex + 2]].texcoord;

    Math::Vector3f p = ray.origin + ray.direction * payload.t;

    float u0 = Math::Length(Math::Cross(v1 - p, v2 - p)) / Math::Length(Math::Cross(v1 - v0, v2 - v0));
    float u1 = Math::Length(Math::Cross(v0 - p, v2 - p)) / Math::Length(Math::Cross(v1 - v0, v2 - v0));
    float u2 = 1.f - u1 - u0;

    payload.normal = Math::Normalize(n0 * u0 + n1 * u1 + n2 * u2);
    payload.texcoord = t0 * u0 + t1 * u1 + t2 * u2;

    auto materialIndices = mesh.GetMaterialIndices();
    const auto &bump = m_Model->GetMaterials()[materialIndices[m_FaceIndex]].bump;
    
    if (bump.GetTexelCount() > 1) {
        auto edge1 = v1 - v0;
        auto edge2 = v2 - v0;
        auto deltaUV1 = t1 - t0;
        auto deltaUV2 = t2 - t0;  

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        Math::Vector3f tangent, bitangent;

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        auto N = Math::Normalize(Math::Cross(edge1, edge2));
        N = Math::Dot(ray.direction, N) > Math::Constants::Epsilon<float> ? -N : N;

        tangent = Math::Normalize(tangent);
        tangent = Math::Normalize(tangent - Math::Dot(tangent, payload.normal) * payload.normal);

        bitangent = Math::Cross(payload.normal, tangent);

        Math::Matrix3f TBN({tangent, bitangent, payload.normal});

        Math::Vector3f normal = bump.PickValue(payload.texcoord);
        normal = 2.f * normal - 1.f;
        normal = Math::Normalize(TBN * normal);

        payload.normal = normal;
    }

    return true;
}