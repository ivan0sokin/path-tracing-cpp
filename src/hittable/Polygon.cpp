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
    // payload.normal = 2.f * material->bump.PickValue(payload.texcoord) - 1.f;

    return true;
}