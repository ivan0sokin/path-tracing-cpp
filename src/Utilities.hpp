#ifndef _UTILITIES_HPP
#define _UTILITIES_HPP

#include <random>
#include <chrono>

#include "../glm/include/glm/vec3.hpp"
#include "../glm/include/glm/vec4.hpp"
#include "../glm/include/glm/geometric.hpp"

namespace Utilities {
	static std::mt19937_64 randomEngine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	static std::uniform_real_distribution<> negativeToOne(-1.f, 1.f); 
	static std::uniform_real_distribution<> negativeHalfToHalf(-0.f, 1.f); 

	static uint32_t randomEngineState = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	constexpr uint32_t RandomUint() {
		uint32_t state = randomEngineState;
		randomEngineState = state * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	constexpr uint32_t ConvertColorToRGBA(glm::vec4 color) noexcept {
        uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		return (a << 24) | (b << 16) | (g << 8) | r;
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

	inline glm::vec3 RandomUnitVector() noexcept {
		return glm::normalize(RandomInUnitSphere());
	}
}

#endif