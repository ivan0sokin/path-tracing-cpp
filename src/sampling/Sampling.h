#ifndef _SAMPLING_H
#define _SAMPLING_H

#include "../math/LAMath.h"

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

    constexpr float GetSamplingGGXPDF(float D, float NdotH, float VdotH) noexcept {
        return D * NdotH / (4.f * VdotH);
    };

    constexpr float DistributionGGX(float roughness, float NdotH) noexcept {
        float alpha = roughness * roughness;
        float alphaSquared = alpha * alpha;

        NdotH = Math::Saturate(NdotH);
        float NdotH2 = NdotH * NdotH;

        float nominator = alphaSquared;
        float denominator = (NdotH2 * (alphaSquared - 1.f) + 1.f);
        denominator = Math::Constants::Pi<float> * denominator * denominator;

        return nominator / denominator;
    }

    constexpr float ComputeDirectFactor(float roughness) noexcept {
        float r = roughness + 1.f;
        return (r * r) / 8.f;
    }

    constexpr float ComputeIBLFactor(float roughness) noexcept {
        return roughness * roughness * 0.5f;
    }

    constexpr float GeometrySchlickGGX(float roughness, float dot) noexcept {
        float k = ComputeIBLFactor(roughness);

        float nominator = dot;
        float denominator = dot * (1.f - k) + k;

        return nominator / denominator;
    }

    constexpr float GeometrySmith(float roughness, float NdotV, float NdotL) noexcept {
        float ggx1 = GeometrySchlickGGX(roughness, NdotV);
        float ggx2 = GeometrySchlickGGX(roughness, NdotL);

        return ggx1 * ggx2;
    }

    constexpr Math::Vector3f FresnelSchlick(float cosTheta, const Math::Vector3f &F0) noexcept {
        return F0 + (1.f - F0) * Math::Pow(1.f - cosTheta, 5.f);
    }

    constexpr Math::Vector3f SampleCookTorranceBRDF(float D, float G, const Math::Vector3f &F, float NdotV, float NdotL) noexcept {
        const float BIAS = 0.001f;

        Math::Vector3f nominator = D * G * F;
        float denominator = 4.f * NdotV * NdotL + BIAS;

        return nominator / denominator;
    }

    constexpr Math::Vector3f SampleLambertianBRDF(const Math::Vector3f &albedo) noexcept {
        return albedo * Math::Constants::InversePi<float>;
    }
}

#endif
