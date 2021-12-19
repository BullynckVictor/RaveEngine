#pragma once
#include <chrono>
#include "Engine/Utility/Types.h"

namespace rv
{
	template<class _Rep, class _Period>
	struct BasicDuration
	{
		BasicDuration(const std::chrono::duration<_Rep, _Period>& duration = {}) : duration(duration) {}

		template<typename T = u64> constexpr T nanos() const { return count<T, std::nano>(); }
		template<typename T = u64> constexpr T micros() const { return count<T, std::micro>(); }
		template<typename T = u64> constexpr T millis() const { return count<T, std::milli>(); }
		template<typename T = float> constexpr T seconds() const { return count<T, std::chrono::seconds::period>(); }
		template<typename T = u64> constexpr T hours() const { return count<T, std::chrono::hours::period>(); }
		template<typename T = u64> constexpr T days() const { return count<T, std::chrono::days::period>(); }
		template<typename T = u64> constexpr T weeks() const { return count<T, std::chrono::weeks::period>(); }
		template<typename T = u64> constexpr T years() const { return count<T, std::chrono::years::period>(); }

		template<typename T, typename P> constexpr T count() const { return std::chrono::duration_cast<std::chrono::duration<T, P>>(duration).count(); }

		std::chrono::duration<_Rep, _Period> duration;
	};

	typedef BasicDuration<u64, std::nano> Duration;

	class Timer
	{
	public:
		Timer();

		Duration Mark();
		Duration Peek() const;
		void Reset();

	private:
		static std::chrono::high_resolution_clock::time_point now();

	private:
		std::chrono::high_resolution_clock::time_point last;
	};
}