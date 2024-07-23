#include "BSDF.h"
#include "../Utilities.hpp"
#include "Sampling.h"

Math::Vector3f BSDF::Sample(const Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept {
    return SampleBRDF(ray, payload, throughput);
}

Math::Vector3f BSDF::SampleBRDF(const Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept {
    Math::Vector3f albedo = m_Material->textures[TextureIndex::Albedo]->PickValue(payload.texcoord);
    float metallic = m_Material->textures[TextureIndex::Metallic]->PickValue(payload.texcoord).r;
    float specular = m_Material->textures[TextureIndex::Specular]->PickValue(payload.texcoord).r;
    float roughness = m_Material->textures[TextureIndex::Roughness]->PickValue(payload.texcoord).r;

    float diffuseRatio = 0.5f * (1.f - metallic);
    float specularRatio = 1.f - diffuseRatio;

    Math::Vector3f N = payload.normal;
    Math::Vector3f V = -ray.direction;

    Math::Vector3f reflectionDirection;
    if (Utilities::RandomFloatInZeroToOne() < diffuseRatio) {
        reflectionDirection = Sampling::SampleHemisphereCosine(N, Utilities::RandomCoordinateVector<2>());
    } else {
        Math::Vector3f halfVec = Sampling::SampleGGX(roughness, N, Utilities::RandomCoordinateVector<2>());
        reflectionDirection = Math::Normalize(2.f * Math::Dot(V, halfVec) * halfVec - V);
    }

    Math::Vector3f L = reflectionDirection;
    Math::Vector3f H = Math::Normalize(V + L);

    float NdotH = Math::Dot(N, H);
    float VdotH = Math::Abs(Math::Dot(V, H));
    float NdotV = Math::Abs(Math::Dot(N, V));
    float NdotL = Math::Abs(Math::Dot(N, L));
    
    Math::Vector3f F0(0.08f, 0.08f, 0.08f);
    F0 = Math::Lerp(F0 * specular, albedo, metallic);

    float D = Sampling::DistributionGGX(roughness, NdotH);
    float G = Sampling::GeometrySmith(roughness, NdotV, NdotL);
    Math::Vector3f F = Sampling::FresnelSchlick(Math::Saturate(Math::Dot(H, V)), F0);

    Math::Vector3f kSpecular = F;
    Math::Vector3f kDiffuse = 1.f - kSpecular;
    kDiffuse *= 1.0 - metallic;

    Math::Vector3f specularBRDF = Sampling::SampleCookTorranceBRDF(D, G, F, NdotV, NdotL);
    float specularPDF = Sampling::GetSamplingGGXPDF(D, Math::Abs(NdotH), VdotH);
    
    Math::Vector3f diffuseBRDF = Sampling::SampleLambertianBRDF(albedo);
    float diffusePDF = Sampling::GetSamplingHemisphereCosinePDF(NdotL);

    Math::Vector3f totalBRDF = (diffuseBRDF * kDiffuse + specularBRDF) * NdotL;
    float totalPDF = diffuseRatio * diffusePDF + specularRatio * specularPDF;

    if (totalPDF > Math::Constants::Epsilon<float>) {
        throughput *= totalBRDF / totalPDF;
    }

    return reflectionDirection;
}

Math::Vector3f BSDF::SampleBTDF(Ray &ray, const HitPayload &payload, Math::Vector3f &throughput) noexcept {
    return Math::Vector3f();
}