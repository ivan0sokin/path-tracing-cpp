#include "Polygon.h"

bool Polygon::Hit(const Ray &ray, float tMin, float tMax, HitPayload &payload) const noexcept {
    auto vertices = m_Mesh->GetVertices();
    auto indices = m_Mesh->GetIndices();

    const auto &p0 = vertices[indices[3 * m_FaceIndex + 0]].position;
    const auto &p1 = vertices[indices[3 * m_FaceIndex + 1]].position;
    const auto &p2 = vertices[indices[3 * m_FaceIndex + 2]].position;

    Math::Vector3f rayCrossEdge2 = Math::Cross(ray.direction, m_Edges[1]);
    float determinant = Math::Dot(m_Edges[0], rayCrossEdge2);

    if (Math::Abs(determinant) < Math::Constants::Epsilon<float>) {
        return false;
    }

    float inverseDeterminant = 1.f / determinant;
    Math::Vector3f s = ray.origin - p0;
    float u = inverseDeterminant * Math::Dot(s, rayCrossEdge2);

    if (u < 0.f || u > 1.f) {
        return false;
    }

    Math::Vector3f sCrossEdge1 = Math::Cross(s, m_Edges[0]);
    float v = inverseDeterminant * Math::Dot(ray.direction, sCrossEdge1);

    if (v < 0.f || u + v > 1.f) {
        return false;
    }

    float t = inverseDeterminant * Math::Dot(m_Edges[1], sCrossEdge1);

    if (t < tMin || tMax < t) {
        return false;
    }    

    auto n0 = vertices[indices[3 * m_FaceIndex + 0]].normal;
    auto n1 = vertices[indices[3 * m_FaceIndex + 1]].normal;
    auto n2 = vertices[indices[3 * m_FaceIndex + 2]].normal;

    n0 = Math::Dot(ray.direction, n0) > Math::Constants::Epsilon<float> ? -n0 : n0;
    n1 = Math::Dot(ray.direction, n1) > Math::Constants::Epsilon<float> ? -n1 : n1;
    n2 = Math::Dot(ray.direction, n2) > Math::Constants::Epsilon<float> ? -n2 : n2;

    const auto &t0 = vertices[indices[3 * m_FaceIndex + 0]].texcoord;
    const auto &t1 = vertices[indices[3 * m_FaceIndex + 1]].texcoord;
    const auto &t2 = vertices[indices[3 * m_FaceIndex + 2]].texcoord;

    Math::Vector3f p = ray.origin + ray.direction * t;

    float u0 = Math::Length(Math::Cross(p1 - p, p2 - p)) / Math::Length(Math::Cross(p1 - p0, p2 - p0));
    float u1 = Math::Length(Math::Cross(p0 - p, p2 - p)) / Math::Length(Math::Cross(p1 - p0, p2 - p0));
    float u2 = 1.f - u1 - u0;

    payload.normal = Math::Normalize(n0 * u0 + n1 * u1 + n2 * u2);
    payload.texcoord = t0 * u0 + t1 * u1 + t2 * u2;

    auto materials = m_Model->GetMaterials();
    auto materialIndices = m_Mesh->GetMaterialIndices();
    auto bump = materials[materialIndices[m_FaceIndex]].textures[TextureIndex::Bump];
    
    if (bump != nullptr) {
        const auto &tan0 = vertices[indices[3 * m_FaceIndex + 0]].tangent;
        const auto &tan1 = vertices[indices[3 * m_FaceIndex + 1]].tangent;
        const auto &tan2 = vertices[indices[3 * m_FaceIndex + 2]].tangent;

        auto tangent = Math::Normalize(tan0 * u0 + tan1 * u1 + tan2 * u2);
        auto bitangent = Math::Cross(payload.normal, tangent);

        Math::Matrix3f TBN({tangent, bitangent, payload.normal});

        Math::Vector3f normal = bump->PickValue(payload.texcoord);
        normal = 2.f * normal - 1.f;
        normal = Math::Normalize(normal * TBN);

        payload.normal = normal;
    }

    payload.t = t;
    payload.material = &materials[materialIndices[m_FaceIndex]];

    return true;
}