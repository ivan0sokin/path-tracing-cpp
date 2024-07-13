#ifndef _SAMPLING_H
#define _SAMPLING_H

#include "../math/Math.h"

namespace Sampling {
    constexpr Math::Vector3f SampleHemisphereCosine(const Math::Vector3f &N, const Math::Vector2f &random) noexcept {
        float cosTheta = Math::Pow(random.x, 1.f / (1.f + 1.f));
		float sinTheta = Math::Sqrt(1.f - cosTheta * cosTheta);
		float phi = Math::Constants::Tau<float> * random.y;
		
		Math::Vector3f localDirection(Math::Cos(phi) * sinTheta, Math::Sin(phi) * sinTheta, cosTheta);

		return Math::Normalize(localDirection * Math::GenerateTangentSpace(N));
    }

    constexpr float GetSamplingHemisphereCosinePDF(float NdotL) noexcept {
        return NdotL * Math::Constants::InversePi<float>;
    }

    constexpr Math::Vector3f SampleGGX(float roughness, const Math::Vector3f &N, const Math::Vector2f &random) noexcept {
        float a = roughness * roughness;

        float phi = Math::Constants::Tau<float> * random.x;
        float cosTheta = Math::Sqrt((1.f - random.y) / (1.f + (a * a - 1.f) * random.y));
        float sinTheta = Math::Sqrt(1.f - cosTheta * cosTheta);

        Math::Vector3f H;
        H.x = Math::Cos(phi) * sinTheta;
        H.y = Math::Sin(phi) * sinTheta;
        H.z = cosTheta;

        Math::Vector3f up = Math::Abs(N.z) < 0.999f ? Math::Vector3f(0.0, 0.0, 1.0) : Math::Vector3f(1.0, 0.0, 0.0);
        Math::Vector3f tangent = Math::Normalize(Math::Cross(up, N));
        Math::Vector3f bitangent = Math::Cross(N, tangent);

        return Math::Normalize(tangent * H.x + bitangent * H.y + N * H.z);
    }

    constexpr float GetSamplingGGXPDF(float NDF, float NdotH, float VdotH) noexcept {
        return NDF * NdotH / (4.f * VdotH);
    };

    constexpr float DistributionGGX(float roughness, float NdotH) noexcept {
        float a = roughness * roughness;
        float a2 = a * a;
        NdotH = Math::Max(NdotH, 0.f);
        float NdotH2 = NdotH * NdotH;

        float nom = a2;
        float denom = (NdotH2 * (a2 - 1.f) + 1.f);
        denom = Math::Constants::Pi<float> * denom * denom;

        return nom / denom;
    }

    constexpr float GeometrySchlickGGX(float roughness, float NdotV) noexcept {
        float r = (roughness + 1.f);
        float k = (r * r) / 8.f;

        float nom = NdotV;
        float denom = NdotV * (1.f - k) + k;

        return nom / denom;
    }

    constexpr float GeometrySmith(float roughness, float NdotV, float NdotL) noexcept {
        float ggx2 = GeometrySchlickGGX(roughness, NdotV);
        float ggx1 = GeometrySchlickGGX(roughness, NdotL);

        return ggx1 * ggx2;
    }

    constexpr Math::Vector3f FresnelSchlick(float cosTheta, const Math::Vector3f &F0) noexcept {
        return F0 + (1.f - F0) * Math::Pow(1.f - cosTheta, 5.f);
    }

    constexpr Math::Vector3f SampleSpecularBRDF(float D, float G, const Math::Vector3f &F, float NdotV, float NdotL) noexcept {        
        Math::Vector3f nominator = D * G * F;
        float denominator = 4.f * NdotV * NdotL + 0.001f;
        Math::Vector3f specularBrdf = nominator / denominator;
        
        return specularBrdf;
    }

    constexpr Math::Vector3f SampleDiffuseBRDF(const Math::Vector3f &albedo) noexcept {
        return albedo * Math::Constants::InversePi<float>;
    }
}

#endif