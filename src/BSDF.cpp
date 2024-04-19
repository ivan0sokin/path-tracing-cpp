#include "BSDF.h"
#include "Utilities.hpp"

Math::Vector3f BSDF::Sample(const Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept {
    float diffuseRatio = 0.5f * (1.f - m_Material->metallic);
    float specularRatio = 1.f - diffuseRatio;

    Math::Vector3f V = Math::Normalize(-ray.direction);

    Math::Vector3f reflectionDirection;
    if (Utilities::RandomFloatInZeroToOne() < diffuseRatio) {
        reflectionDirection = Utilities::RandomInHemisphere(payload.normal);
    } else {
        Math::Vector3f halfVec;
        {
            Math::Vector2f Xi{Utilities::RandomFloatInZeroToOne(), Utilities::RandomFloatInZeroToOne()};
            Math::Vector3f N = payload.normal;

            float a = m_Material->roughness * m_Material->roughness;

            float phi = Math::Constants::Tau<float> * Xi.x;
            float cosTheta = Math::Sqrt((1.f - Xi.y) / (1.f + (a * a - 1.f) * Xi.y));
            float sinTheta = Math::Sqrt(1.f - cosTheta * cosTheta);

            Math::Vector3f H;
            H.x = Math::Cos(phi) * sinTheta;
            H.y = Math::Sin(phi) * sinTheta;
            H.z = cosTheta;

            Math::Vector3f up = Math::Abs(N.z) < 0.999f ? Math::Vector3f(0.0, 0.0, 1.0) : Math::Vector3f(1.0, 0.0, 0.0);
            Math::Vector3f tangent = Math::Normalize(Math::Cross(up, N));
            Math::Vector3f bitangent = Math::Cross(N, tangent);

            halfVec = tangent * H.x + bitangent * H.y + N * H.z;
            halfVec = Math::Normalize(halfVec);
        }

        reflectionDirection = 2.f * Math::Dot(V, halfVec) * halfVec - V;
    }

    auto DistributionGGX = [](const Math::Vector3f &N, const Math::Vector3f &H, float roughness) {
        float a = roughness * roughness;
        float a2 = a * a;
        float NdotH = Math::Max(Math::Dot(N, H), 0.f);
        float NdotH2 = NdotH * NdotH;

        float nom = a2;
        float denom = (NdotH2 * (a2 - 1.f) + 1.f);
        denom = Math::Constants::Pi<float> * denom * denom;

        return nom / denom;
    };

    auto GeometrySchlickGGX = [](float NdotV, float roughness) {
        float r = (roughness + 1.f);
        float k = (r * r) / 8.f;

        float nom = NdotV;
        float denom = NdotV * (1.f - k) + k;

        return nom / denom;
    };

    auto GeometrySmith = [&](const Math::Vector3f &N, const Math::Vector3f &V, const Math::Vector3f &L, float roughness) {
        float NdotV = Math::Abs(Math::Dot(N, V));
        float NdotL = Math::Abs(Math::Dot(N, L));
        float ggx2 = GeometrySchlickGGX(NdotV, roughness);
        float ggx1 = GeometrySchlickGGX(NdotL, roughness);

        return ggx1 * ggx2;
    };

    auto FresnelSchlick = [](float cosTheta, const Math::Vector3f &F0) {
        return F0 + (1.f - F0) * pow(1.f - cosTheta, 5.f);
    };

    auto SpecularBRDF = [](float D, float G, const Math::Vector3f &F, const Math::Vector3f &V, const Math::Vector3f &L, const Math::Vector3f &N) {        
        float NdotL = Math::Abs(Math::Dot(N, L));
        float NdotV = Math::Abs(Math::Dot(N, V));
                
        Math::Vector3f nominator = D * G * F;
        float denominator = 4.f * NdotV * NdotL + 0.001f;
        Math::Vector3f specularBrdf = nominator / denominator;
        
        return specularBrdf;
    };

    auto DiffuseBRDF = [](const Math::Vector3f &albedo) {
        return albedo * Math::Constants::InversePi<float>;
    };

    auto ImportanceSampleGGXPDF = [](float NDF, float NdotH, float VdotH) {
        return NDF * NdotH / (4.f * VdotH);
    };

    auto CosineSamplingPDF = [](float NdotL) {
        return NdotL * Math::Constants::InversePi<float>;
    };

    Math::Vector3f L = Math::Normalize(reflectionDirection);
    Math::Vector3f H = Math::Normalize(V + L);

    float NdotL = Math::Abs(Math::Dot(payload.normal, L));
    float NdotH = Math::Abs(Math::Dot(payload.normal, H));
    float VdotH = Math::Abs(Math::Dot(V, H));
    
    float NdotV = Math::Abs(Math::Dot(payload.normal, V));
    
    Math::Vector3f F0 = Math::Vector3f(0.08f, 0.08f, 0.08f);
    F0 = Math::Lerp(F0 * m_Material->specular, m_Material->albedo, m_Material->metallic);

    float NDF = DistributionGGX(payload.normal, H, m_Material->roughness);
    float G = GeometrySmith(payload.normal, V, L, m_Material->roughness);
    Math::Vector3f F = FresnelSchlick(Math::Max(Math::Dot(H, V), 0.f), F0);

    Math::Vector3f kS = F;
    Math::Vector3f kD = 1.f - kS;
    kD *= 1.0 - m_Material->metallic;

    Math::Vector3f specularBrdf = SpecularBRDF(NDF, G, F, V, L, payload.normal);
        
    float specularPdf = ImportanceSampleGGXPDF(NDF, NdotH, VdotH);
    
    Math::Vector3f diffuseBrdf = DiffuseBRDF(m_Material->albedo);
    float diffusePdf = CosineSamplingPDF(NdotL);

    Math::Vector3f totalBrdf = (diffuseBrdf * kD + specularBrdf) * NdotL;
    float totalPdf = diffuseRatio * diffusePdf + specularRatio * specularPdf;


    if (totalPdf > Math::Constants::Epsilon<float>) {
        throughput *= totalBrdf / totalPdf;
    }

    return reflectionDirection;
}