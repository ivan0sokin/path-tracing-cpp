#ifndef _UTILITIES_HPP
#define _UTILITIES_HPP

#include <random>
#include <chrono>
#include <numbers>

#include "../glm/include/glm/vec3.hpp"
#include "../glm/include/glm/vec4.hpp"
#include "../glm/include/glm/geometric.hpp"

namespace Utilities {
	static uint32_t s_RandomEngineState = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	
	constexpr uint32_t RandomUint() {
		uint32_t state = s_RandomEngineState;
		s_RandomEngineState = state * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	constexpr float RandomFloatInZeroToOne() noexcept {
		return (float)RandomUint() / UINT32_MAX;
	}

	constexpr float RandomFloatInNegativeHalfToHalf() noexcept {
		return RandomFloatInZeroToOne() - 0.5f;
	}

	constexpr float RandomFloatInNegativeToOne() noexcept {
		return RandomFloatInZeroToOne() * 2.f - 1.f;
	}

	constexpr glm::vec3 RandomInUnitSphere() noexcept {
		while (true) {
			glm::vec3 result(RandomFloatInNegativeToOne(), RandomFloatInNegativeToOne(), RandomFloatInNegativeToOne());
			if (glm::dot(result, result) < 1.f) {
				return result;
			}
		}
	}

	constexpr glm::vec3 RandomUnitVector() noexcept {
		glm::vec3 v = RandomInUnitSphere();
		return v * (1.f / glm::sqrt(glm::dot(v, v)));
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

	constexpr glm::vec3 RandomUnitVectorFast() noexcept {
		glm::vec3 v = RandomInUnitSphere();
		return v * InverseSqrtFast(glm::dot(v, v));
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

	constexpr uint32_t ConvertColorToRGBA(glm::vec4 color) noexcept {
        uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		return (a << 24) | (b << 16) | (g << 8) | r;
    }

	constexpr glm::vec4 CorrectGamma(const glm::vec4 &color, float inverseGamma) {
		return {
			glm::pow(color.r, inverseGamma),
			glm::pow(color.g, inverseGamma),
			glm::pow(color.b, inverseGamma),
			color.a
		};
	}

	constexpr glm::vec4 CorrectGammaFast(const glm::vec4 &color, float inverseGamma) {
		return {
			PowFast(color.r, inverseGamma),
			PowFast(color.g, inverseGamma),
			PowFast(color.b, inverseGamma),
			color.a
		};
	}

	constexpr bool AlmostZero(const glm::vec3 &v) {
		constexpr float epsilon = std::numeric_limits<float>::epsilon();
		return glm::abs(v.x) < epsilon && glm::abs(v.y) < epsilon && glm::abs(v.z) < epsilon;
	}
}

#endif