#pragma once
#include "Engine/Core/CompileTimeInfo.h"
#include "Engine/Utility/Types.h"
#include <random>
#include <limits>

namespace rv
{
	namespace detail
	{
		template<size_t S>
		struct RNGEngineT {};

		template<> struct RNGEngineT<4> { using Engine = std::mt19937; };
		template<> struct RNGEngineT<8> { using Engine = std::mt19937_64; };

		using RNGEngine = RNGEngineT<cti.architecture.pointer_size>::Engine;
	}

	class RandomNumberGenerator
	{
	public:
		RandomNumberGenerator() : rng(std::random_device()()) {}

		template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
		T random()
		{
			if constexpr (std::is_integral_v<T>)
				return random<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
			else
				return std::uniform_real_distribution<T>()(rng);
		}
		template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
		T random(const T& end)
		{
			return random(0, end);
		}
		template<typename T>
		requires std::is_integral_v<T> || std::is_floating_point_v<T>
		T random(const T& begin, const T& end)
		{
			if constexpr (std::is_integral_v<T>)

				if constexpr (sizeof(T) == 1)
					return (T)std::uniform_int_distribution<u16>((u16)begin, (u16)end)(rng);
				else
					return std::uniform_int_distribution<T>(begin, end)(rng);

			else
				return std::uniform_real_distribution<T>(begin, end)(rng);
		}
	
	private:
		detail::RNGEngine rng;
	};

	extern RandomNumberGenerator rng;
}