#ifndef _UTILITIES_HPP
#define _UTILITIES_HPP

#include <chrono>
#include <random>

#include "math/Math.h"

namespace Utilities {
	inline static uint32_t s_RandomEngineState = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	thread_local inline static std::mt19937_64 s_RandomNumberGenerator(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	inline static std::uniform_real_distribution<> s_ZeroToOne(0.f, 1.f);

	inline uint32_t RandomUint() {
		uint32_t state = s_RandomEngineState;
		s_RandomEngineState = state * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	inline float RandomFloatInZeroToOne() noexcept {
		return s_ZeroToOne(s_RandomNumberGenerator);
	}

	inline float RandomFloatInNegativeHalfToHalf() noexcept {
		return RandomFloatInZeroToOne() - 0.5f;
	}

	inline float RandomFloatInNegativeToOne() noexcept {
		return RandomFloatInZeroToOne() * 2.f - 1.f;
	}

	inline int RandomIntInRange(int min, int max) noexcept {
		return std::uniform_int_distribution<>(min, max)(s_RandomNumberGenerator);
	}

	inline Math::Vector3f RandomInUnitSphere() noexcept {
		while (true) {
			Math::Vector3f result(RandomFloatInNegativeToOne(), RandomFloatInNegativeToOne(), RandomFloatInNegativeToOne());
			if (Math::Dot(result, result) < 1.f) {
				return result;
			}
		}
	}

	inline Math::Vector3f RandomUnitVector() noexcept {
		return Math::Normalize(RandomInUnitSphere());
	}

	// inline Math::Vector3f RandomInHemisphere(const Math::Vector3f &normal) {
	// 	Math::Vector3f randomUnitVector = RandomUnitVector();

	// 	if (Math::Dot(randomUnitVector, normal) <= 0.f) {
	// 		return -randomUnitVector;
	// 	}

	// 	return randomUnitVector;
	// }

	inline Math::Vector3f RandomInHemisphere(const Math::Vector3f &normal) {
		float cosTheta = Math::Pow(Utilities::RandomFloatInZeroToOne(), 1.0f / (1.f + 1.0f));
		float sinTheta = Math::Sqrt(1.0f - cosTheta * cosTheta);
		float phi = Math::Constants::Tau<float> * Utilities::RandomFloatInZeroToOne();
		Math::Vector3f tangentSpaceDir = Math::Vector3f(Math::Cos(phi) * sinTheta, Math::Sin(phi) * sinTheta, cosTheta);

		// Transform direction to be centered around whatever noraml we need
		Math::Vector3f helper = Math::Vector3f(1, 0, 0);
		if (Math::Abs(normal.x) > 0.99f)
			helper = Math::Vector3f(0, 0, 1);

		// Generate vectors
		Math::Vector3f tangent = Math::Normalize(Math::Cross(normal, helper));
		Math::Vector3f binormal = Math::Normalize(Math::Cross(normal, tangent));

		return tangent * tangentSpaceDir.x + binormal * tangentSpaceDir.y + normal * tangentSpaceDir.z; 
	}

	constexpr float InverseSqrtFast(float x) {
		constexpr float threeHalfs = 1.5f;

		float halfX = x * 0.5f;
		uint32_t i = *(uint32_t*)&x;
		i = 0x5f3759df - (i >> 1);

		x = *(float*)&i;
		x = x * (threeHalfs - halfX * x * x);

		return x;
	}

	inline Math::Vector3f RandomUnitVectorFast() noexcept {
		Math::Vector3f v = RandomInUnitSphere();
		return v * InverseSqrtFast(Math::Dot(v, v));
	}

	inline Math::Vector3f RandomInHemisphereFast(const Math::Vector3f &normal) {
    	Math::Vector3f randomUnitVector = RandomUnitVectorFast();
		if (Math::Dot(randomUnitVector, normal) < 0.f) {
			return -randomUnitVector;
		}

		return randomUnitVector;
	}

	inline Math::Vector3f RandomCosineDirection() {
		float r1 = RandomFloatInZeroToOne();
		float r2 = RandomFloatInZeroToOne();

		constexpr float twoPi = 2.f * std::numbers::pi;

		float phi = twoPi * r1;
		float x = cos(phi) * sqrt(r2);
		float y = sin(phi) * sqrt(r2);
		float z = sqrt(1.f - r2);

		return {x, y, z};
	}

	constexpr uint32_t AsUint(float x) {
		return *(uint32_t*)&x;
	}

	constexpr float AsFloat(uint32_t x) {
		return *(float*)&x;
	}

	constexpr float PowFast(float x, float exp) {
		constexpr float oneAsUint = 0x3f800000u;
		return AsFloat(int(exp * (AsUint(x) - oneAsUint)) + oneAsUint);
	}

	constexpr bool AlmostZero(const Math::Vector3f &v) {
		constexpr float epsilon = std::numeric_limits<float>::epsilon();
		return Math::Abs(v.x) < epsilon && Math::Abs(v.y) < epsilon && Math::Abs(v.z) < epsilon;
	}

	constexpr uint32_t ConvertColorToRGBA(const Math::Vector4f &color) noexcept {
        uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		return (a << 24) | (b << 16) | (g << 8) | r;
    }

	constexpr Math::Vector4f CorrectGamma(const Math::Vector4f &color, float inverseGamma) {
		return {
			Math::Pow(color.r, inverseGamma),
			Math::Pow(color.g, inverseGamma),
			Math::Pow(color.b, inverseGamma),
			color.a
		};
	}

	constexpr Math::Vector4f CorrectGammaFast(const Math::Vector4f &color, float inverseGamma) {
		return {
			PowFast(color.r, inverseGamma),
			PowFast(color.g, inverseGamma),
			PowFast(color.b, inverseGamma),
			color.a
		};
	}
}

#endif