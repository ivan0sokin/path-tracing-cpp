#include "../assets/Model.h"
#include "Polygon.h"

Shapes::Triangle Polygon::MakeTriangle() const noexcept {
    auto mesh = m_Model->GetMeshes()[m_MeshIndex];
    
    auto vertices = mesh->GetVertices();
    auto indices = mesh->GetIndices();

    auto materials = m_Model->GetMaterials();
    auto materialIndices = mesh->GetMaterialIndices();
        
    return Shapes::Triangle(vertices[indices[3 * m_FaceIndex + 0]].position, vertices[indices[3 * m_FaceIndex + 1]].position, vertices[indices[3 * m_FaceIndex + 2]].position, &materials[materialIndices[m_FaceIndex]]);
}

bool Polygon::Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept {
    if (!MakeTriangle().Hit(ray, tMin, tMax, payload)) {
        return false;
    }

    auto mesh = m_Model->GetMeshes()[m_MeshIndex];

    auto vertices = mesh->GetVertices();
    auto indices = mesh->GetIndices();

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

    auto materialIndices = mesh->GetMaterialIndices();
    auto bump = m_Model->GetMaterials()[materialIndices[m_FaceIndex]].textures[TextureIndex::Bump];
    
    if (bump != nullptr) {
        auto tan0 = vertices[indices[3 * m_FaceIndex + 0]].tangent;
        auto tan1 = vertices[indices[3 * m_FaceIndex + 1]].tangent;
        auto tan2 = vertices[indices[3 * m_FaceIndex + 2]].tangent;

        auto tangent = Math::Normalize(tan0 * u0 + tan1 * u1 + tan2 * u2);
        auto bitangent = Math::Cross(payload.normal, tangent);

        Math::Matrix3f TBN({tangent, bitangent, payload.normal});

        Math::Vector3f normal = bump->PickValue(payload.texcoord);
        normal = 2.f * normal - 1.f;
        normal = Math::Normalize(normal * TBN);

        payload.normal = normal;
    }

    return true;
}