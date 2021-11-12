#pragma once
#include "Engine/Utility/Types.h"
#include <limits>
#include <type_traits>

namespace rv
{
	template<typename C, typename T>
	concept OptionalCheckerConcept = requires(T value)
	{
		C::InvalidValue();
		C::Valid(value);
		C::InValid(value);
	};

	template<typename T>
	struct DefaultChecker
	{
		static constexpr T InvalidValue()
		{
			if constexpr (std::is_floating_point_v<T>)
				return std::numeric_limits<T>::quit_NaN();

			else if constexpr (std::is_integral_v<T>)
				if constexpr (std::is_signed_v<T>)
					return std::numeric_limits<T>::min();
				else
					return std::numeric_limits<T>::max();

			else if constexpr (std::is_pointer_v<T>)
				return nullptr;

			else
				static_assert(false, "no default implementation of InvalidValue<T>()");
		}

		static constexpr bool Valid(const T& value)		{ return value != InvalidValue(); } 
		static constexpr bool InValid(const T& value)	{ return value == InvalidValue(); }
	};

	template<typename T, OptionalCheckerConcept<T> C = DefaultChecker<T>>
	struct Optional
	{
		static constexpr T invalid_value = C::InvalidValue();

		constexpr Optional() : value(C::InvalidValue()) {}
		constexpr Optional(const T& value) : value(value) {}
		constexpr Optional(T&& value) : value(std::move(value)) {}

		constexpr operator bool() const { return valid(); }

		constexpr bool valid() const { return C::Valid(value); }
		constexpr bool invalid() const { return C::InValid(value); }

		T value;
	};

	typedef Optional<size_t> OIndex;
	typedef Optional<u32> OIndex32;
	typedef Optional<u64> OIndex64;
}