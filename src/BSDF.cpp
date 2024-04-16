#include "BSDF.h"
#include "Utilities.hpp"

std::pair<Math::Vector3f, Math::Vector3f> BSDF::Sample(const Ray &ray, const HitPayload &payload) noexcept {
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
            float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
            float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

            // from spherical coordinates to cartesian coordinates
            Math::Vector3f H;
            H.x = cos(phi) * sinTheta;
            H.y = sin(phi) * sinTheta;
            H.z = cosTheta;

            // from tangent-space vector to world-space sample vector
            Math::Vector3f up = Math::Abs(N.z) < 0.999f ? Math::Vector3f(0.0, 0.0, 1.0) : Math::Vector3f(1.0, 0.0, 0.0);
            Math::Vector3f tangent = Math::Normalize(Math::Cross(up, N));
            Math::Vector3f bitangent = Math::Cross(N, tangent);

            halfVec = tangent * H.x + bitangent * H.y + N * H.z;
            halfVec = Math::Normalize(halfVec);
        }

        reflectionDirection = Math::Normalize(2.f * Math::Dot(V, halfVec) * halfVec - V);
    }

    auto DistributionGGX = [](const Math::Vector3f &N, const Math::Vector3f &H, float roughness) {
        float a = roughness * roughness;
        float a2 = a * a;
        float NdotH = Math::Max(Math::Dot(N, H), 0.f);
        float NdotH2 = NdotH * NdotH;

        float nom = a2;
        float denom = (NdotH2 * (a2 - 1.0) + 1.0);
        denom = Math::Constants::Pi<float> * denom * denom;

        return nom / denom;
    };

    auto GeometrySchlickGGX = [](float NdotV, float roughness) {
        float r = (roughness + 1.0);
        float k = (r * r) / 8.0;

        float nom = NdotV;
        float denom = NdotV * (1.0 - k) + k;

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
                
        //specualr
        //Microfacet specular = D * G * F / (4 * NoL * NoV)
        Math::Vector3f nominator = D * G * F;
        float denominator = 4.0 * NdotV * NdotL + 0.000001f;
        Math::Vector3f specularBrdf = nominator / denominator;
        
        return specularBrdf;
    };
    

    auto DiffuseBRDF = [](const Math::Vector3f &albedo) {
        return albedo * Math::Constants::InversePi<float>;
    };

    auto ImportanceSampleGGX_PDF = [](float NDF, float NdotH, float VdotH) {
        //ImportanceSampleGGX pdf
            //pdf = D * NoH / (4 * VoH)
        return NDF * NdotH / (4 * VdotH);
    };

    auto CosinSamplingPDF = [](float NdotL) {
        return NdotL * Math::Constants::InversePi<float>;
    };

    Math::Vector3f L = Math::Normalize(reflectionDirection);
    Math::Vector3f H = Math::Normalize(V + L);

    float NdotL = Math::Abs(Math::Dot(payload.normal, L));
    float NdotH = Math::Abs(Math::Dot(payload.normal, H));
    float VdotH = Math::Abs(Math::Dot(V, H));
    
    float NdotV = Math::Abs(Math::Dot(payload.normal, V));
    
    Math::Vector3f F0 = Math::Vector3f(0.08, 0.08, 0.08);
    F0 = Math::Lerp(F0 * m_Material->specular, m_Material->albedo, m_Material->metallic);

    float NDF = DistributionGGX(payload.normal, H, m_Material->roughness);
    float G = GeometrySmith(payload.normal, V, L, m_Material->roughness);
    Math::Vector3f F = FresnelSchlick(Math::Max(Math::Dot(H, V), 0.f), F0);

    Math::Vector3f kS = F;
    Math::Vector3f kD = 1.f - kS;
    kD *= 1.0 - m_Material->metallic;

    Math::Vector3f specularBrdf = SpecularBRDF(NDF, G, F, V, L, payload.normal);
        
    //hemisphere sampling pdf
    //pdf = 1 / (2 * PI)
    //float speccualrPdf = 1 / (2 * PI);
    
    //ImportanceSampleGGX pdf
    //pdf = D * NoH / (4 * VoH)
    float specularPdf = ImportanceSampleGGX_PDF(NDF, NdotH, VdotH);
    
    //diffuse
    //Lambert diffuse = diffuse / PI
    Math::Vector3f diffuseBrdf = DiffuseBRDF(m_Material->albedo);
    //cosin sample pdf = N dot L / PI
    float diffusePdf = CosinSamplingPDF(NdotL);

    Math::Vector3f totalBrdf = (diffuseBrdf * kD + specularBrdf) * NdotL; // delete kS
    float totalPdf = diffuseRatio * diffusePdf + specularRatio * specularPdf;

    // return {m_Material->albedo, reflectionDirection}; // goida)

    if (totalPdf > 0.f) {
        return {totalBrdf / totalPdf, reflectionDirection};
    }

    return {Math::Vector3f(0.f), reflectionDirection};
}