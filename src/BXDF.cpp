#include "BXDF.h"
#include "Utilities.hpp"
#include <cassert>

Math::Vector3f BXDF::Sample(Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept {
    return SampleBRDF(ray, payload, throughput);
    // float transparency = m_Material->transparency;
    
    // if (Utilities::RandomFloatInZeroToOne() < 1.f - m_Material->refract.PickValue(payload.texcoord).r) {
    //     return SampleBRDF(ray, payload, throughput); 
    // }

    // return SampleBSDF(ray, payload, throughput);
}

Math::Vector3f BXDF::SampleBRDF(const Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept {
    Math::Vector3f albedo = m_Material->albedo.PickValue(payload.texcoord);
    float metallic = m_Material->metallic.PickValue(payload.texcoord).r;
    float roughness = m_Material->roughness.PickValue(payload.texcoord).r;
    float specular = m_Material->specular.PickValue(payload.texcoord).r;

    float diffuseRatio = 0.5f * (1.f - metallic);
    float specularRatio = 1.f - diffuseRatio;

    Math::Vector3f V = -ray.direction;

    Math::Vector3f reflectionDirection;
    if (Utilities::RandomFloatInZeroToOne() < diffuseRatio) {
        reflectionDirection = Utilities::RandomInHemisphere(payload.normal);
    } else {
        Math::Vector3f halfVec;
        {
            Math::Vector2f Xi{Utilities::RandomFloatInZeroToOne(), Utilities::RandomFloatInZeroToOne()};
            Math::Vector3f N = payload.normal;

            float a = roughness * roughness;

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

        reflectionDirection = Math::Normalize(2.f * Math::Dot(V, halfVec) * halfVec - V);
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
        return F0 + (1.f - F0) * Math::Pow(1.f - cosTheta, 5.f);
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

    Math::Vector3f L = reflectionDirection;
    Math::Vector3f H = Math::Normalize(V + L);

    float NdotL = Math::Abs(Math::Dot(payload.normal, L));
    float NdotH = Math::Abs(Math::Dot(payload.normal, H));
    float VdotH = Math::Abs(Math::Dot(V, H));
    
    float NdotV = Math::Abs(Math::Dot(payload.normal, V));
    
    Math::Vector3f F0 = Math::Vector3f(0.08f, 0.08f, 0.08f);
    F0 = Math::Lerp(F0 * specular, albedo, metallic);

    float NDF = DistributionGGX(payload.normal, H, roughness);
    float G = GeometrySmith(payload.normal, V, L, roughness);
    Math::Vector3f F = FresnelSchlick(Math::Max(Math::Dot(H, V), 0.f), F0);

    Math::Vector3f kS = F;
    Math::Vector3f kD = 1.f - kS;
    kD *= 1.0 - metallic;

    Math::Vector3f specularBrdf = SpecularBRDF(NDF, G, F, V, L, payload.normal);
        
    float specularPdf = ImportanceSampleGGXPDF(NDF, NdotH, VdotH);
    
    Math::Vector3f diffuseBrdf = DiffuseBRDF(albedo);
    float diffusePdf = CosineSamplingPDF(NdotL);

    Math::Vector3f totalBrdf = (diffuseBrdf * kD + specularBrdf) * NdotL;
    float totalPdf = diffuseRatio * diffusePdf + specularRatio * specularPdf;

    Math::Vector3f hitPoint = ray.origin + ray.direction * payload.t;

    // ray.origin = hitPoint + payload.normal * 0.001f;
    // ray.direction = reflectionDirection;

    if (totalPdf > Math::Constants::Epsilon<float>) {
        throughput *= totalBrdf / totalPdf;
    }

    return reflectionDirection;
}

Math::Vector3f BXDF::SampleBSDF(Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept {
    Math::Vector3f albedo = m_Material->albedo.PickValue(payload.texcoord);
    float metallic = m_Material->metallic.PickValue(payload.texcoord).r;
    float roughness = m_Material->roughness.PickValue(payload.texcoord).r;
    float specular = m_Material->specular.PickValue(payload.texcoord).r;
    
    Math::Vector3f hitPoint = ray.origin + ray.direction * payload.t;

    bool fromOutside = Math::Dot(ray.direction, payload.normal) < 0.f;
    Math::Vector3f N = fromOutside ? payload.normal : -payload.normal;
    Math::Vector3f bias = N * 0.001f;
    
    float etai = ray.opticalDensity;
    float etat = m_Material->refractionIndex;
    
    auto ImportanceSampleGGX = [](const Math::Vector2f &Xi, const Math::Vector3f &N, const Math::Vector3f &V, float roughness){
        float a = roughness * roughness;

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

        return Math::Normalize(tangent * H.x + bitangent * H.y + N * H.z);
    };

    auto FresnelSchlick = [](float cosTheta, const Math::Vector3f &F0) {
        return F0 + (1.f - F0) * Math::Pow(1.f - cosTheta, 5.f);
    };

    auto Calculatefresnel = [](const Math::Vector3f &I, const Math::Vector3f &N, float etai, float etat, float ior) {
        float kr;
        float cosi = Math::Clamp(Math::Dot(I, N), -1.f, 1.f);
        if (cosi > 0.f) {
            std::swap(etai, etat);
        }
        // Compute sini using Snell's law
        float sint = etai / etat * Math::Sqrt(Math::Max(0.f, 1.f - cosi * cosi));
        // Total internal reflection
        if (sint >= 1)
        {
            kr = 1;
        }
        else
        {
            float cost = Math::Sqrt(Math::Max(0.f, 1 - sint * sint));
            cosi = Math::Abs(cosi);
            float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
            float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
            kr = (Rs * Rs + Rp * Rp) / 2;
        }
        return kr;
        // As a consequence of the conservation of energy, transmittance is given by:
        // kt = 1 - kr;
    };

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

    auto SpecularBRDF = [](float D, float G, const Math::Vector3f &F, const Math::Vector3f &V, const Math::Vector3f &L, const Math::Vector3f &N) {        
        float NdotL = Math::Abs(Math::Dot(N, L));
        float NdotV = Math::Abs(Math::Dot(N, V));
                
        Math::Vector3f nominator = D * G * F;
        float denominator = 4.f * NdotV * NdotL + 0.001f;
        Math::Vector3f specularBrdf = nominator / denominator;
        
        return specularBrdf;
    };

    auto ImportanceSampleGGX_PDF = [](float NDF, float NdotH, float VdotH) {
        return NDF * NdotH / (4 * VdotH);
    };

    auto RefractionBTDF = [](float D, float G, const Math::Vector3f &F, const Math::Vector3f &V, const Math::Vector3f &L, const Math::Vector3f &N, const Math::Vector3f &H, float etaIn, float etaOut) { //Not reciprocal! be careful about direction!
        
        float NdotL = Math::Abs(Math::Dot(N, L));
        float NdotV = Math::Abs(Math::Dot(N, V));
                
        float VdotH = Math::Abs(Math::Dot(V, H));
        float LdotH = Math::Abs(Math::Dot(L, H));
                
        
        float term1 = VdotH * LdotH / (NdotV * NdotL);
        Math::Vector3f term2 = etaOut * etaOut * (1 - F) * G * D;
                //term1 = 1;
                //term2 = 1;
        float term3 = (etaIn * VdotH + etaOut * LdotH) * (etaIn * VdotH + etaOut * LdotH) + 0.001f;
                //term3 = 1;
        Math::Vector3f refractionBrdf = term1 * term2 / term3;
        
        return refractionBrdf;
    };

    Math::Vector3f V = Math::Normalize(-ray.direction);
    Math::Vector3f H = ImportanceSampleGGX(Math::Vector2f(Utilities::RandomFloatInZeroToOne(), Utilities::RandomFloatInZeroToOne()), N, V, roughness);
    
    Math::Vector3f F0 = Math::Vector3f(0.08, 0.08, 0.08);
    F0 = F0 * specular;
    Math::Vector3f F = FresnelSchlick(Math::Max(Math::Dot(H, V), 0.f), F0);
    
    float kr = Calculatefresnel(ray.direction, payload.normal, etai, etat, 1.55f);
    
    float specularRoatio = kr;
    float refractionRatio = 1 - kr;
    
    Math::Vector3f L;
    
    if (Utilities::RandomFloatInZeroToOne() <= specularRoatio)
    {
        ray.origin = hitPoint + bias;
        L = Math::Reflect(ray.direction, H);
        ray.direction = L;
    }
    else
    {
        float eta = fromOutside ? etai / etat : etat / etai;
        auto R = Math::Refract(ray.direction, H, eta);
        if (R == Math::Vector3f(0.f, 0.f, 0.f)) {
            L = ray.direction;
        } else {
            L = Math::Normalize(R);
        }

        ray.origin = hitPoint - bias;
        ray.direction = L;
        //L = N;
        if (!fromOutside) {
            //since the BTDF is not reciprocal, we need to invert the direction of our vectors.

            std::swap(L, V);
                
            N = -N;
            H = -H;
        }
    }
    
    float NdotL = Math::Abs(Math::Dot(N, L));
    float NdotV = Math::Abs(Math::Dot(N, V));
    
    float NdotH = Math::Abs(Math::Dot(N, H));
    float VdotH = Math::Abs(Math::Dot(V, H));
    float LdotH = Math::Abs(Math::Dot(L, H));
    
    
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    
        //specualr

    Math::Vector3f specularBrdf = SpecularBRDF(NDF, G, F, V, L, N);
    
    //ImportanceSampleGGX pdf
    //pdf = D * NoH / (4 * VoH)
    float speccualrPdf = ImportanceSampleGGX_PDF(NDF, NdotH, VdotH);
    
    //refraction
    float etaOut = etat;
    float etaIn = etai;
    
    Math::Vector3f refractionBtdf = RefractionBTDF(NDF, G, F, V, L, N, H, etaIn, etaOut);
    float refractionPdf = ImportanceSampleGGX_PDF(NDF, NdotH, VdotH);

    //BSDF = BRDF + BTDF
    // Math::Vector3f totalBrdf = (specularBrdf + refractionBtdf * hit.transColor) * NdotL;
    Math::Vector3f totalBrdf = (specularBrdf + refractionBtdf * Math::Vector3f(1.f)) * NdotL;
    float totalPdf = specularRoatio * speccualrPdf + refractionRatio * refractionPdf;
    
    // ray.opticalDensity = etaOut;

    if (totalPdf > Math::Constants::Epsilon<float>) {
        throughput *= totalBrdf / totalPdf;
    }

    return Math::Vector3f(0.f);
}