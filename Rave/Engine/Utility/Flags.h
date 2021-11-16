#pragma once
#include "Engine/Utility/Types.h"
#include <type_traits>

namespace rv
{
	template<typename E, typename D = size_t>
		requires std::is_enum_v<E> && std::is_unsigned_v<D>
	struct Flags
	{
	public:
		constexpr Flags() : m_data(0) {}
		constexpr Flags(E flag) : m_data((D)0x1 << flag) {}
		constexpr Flags(const Flags& rhs) : m_data(rhs.m_data) {}
		constexpr Flags(Flags&& rhs) : m_data(rhs.m_data) { rhs.m_data = 0; }

		constexpr Flags& operator= (E flag) { m_data = (D)0x1 << flag; return *this; }
		constexpr Flags& operator= (const Flags& rhs) { m_data = rhs.m_data; return *this; }
		constexpr Flags& operator= (Flags&& rhs) { m_data = rhs.m_data; rhs.m_data = 0; return *this; }

		constexpr bool operator== (const Flags& rhs) const { return m_data == rhs.m_data; }
		constexpr bool operator!= (const Flags& rhs) const { return m_data != rhs.m_data; }
		constexpr bool operator<= (const Flags& rhs) const { return m_data <= rhs.m_data; }
		constexpr bool operator>= (const Flags& rhs) const { return m_data >= rhs.m_data; }
		constexpr bool operator<  (const Flags& rhs) const { return m_data <  rhs.m_data; }
		constexpr bool operator>  (const Flags& rhs) const { return m_data >  rhs.m_data; }

		constexpr Flags  operator|  (const Flags& rhs) { return m_data | rhs.m_data; }
		constexpr Flags  operator&  (const Flags& rhs) { return m_data & rhs.m_data; }
		constexpr Flags  operator^  (const Flags& rhs) { return m_data ^ rhs.m_data; }
		constexpr Flags& operator|= (const Flags& rhs) { m_data |= rhs.m_data; return *this; }
		constexpr Flags& operator&= (const Flags& rhs) { m_data &= rhs.m_data; return *this; }
		constexpr Flags& operator^= (const Flags& rhs) { m_data ^= rhs.m_data; return *this; }

		constexpr bool contain(E flag) const { return m_data & ((D)0x1 << flag); }
		constexpr bool contain(Flags flags) const { return (m_data & flags.m_data) == flags.m_data; }
		constexpr bool only_contains(E flag) const { return m_data == ((D)0x1 << flag); }
		constexpr bool only_contains(Flags flags) const { return m_data == flags.m_data; }

		constexpr Flags inverse() const { return ~m_data; }
		constexpr void invert() { m_data = ~m_data; }

		constexpr void clear() { m_data = 0; }

		constexpr D data() const { return m_data; }

	private:
		constexpr Flags(D data) : m_data(data) {}

		D m_data;
	};

	template<typename E, typename D = size_t, typename... Args> static constexpr Flags<E, D> combine(const E& flag, const Args&... args)
	{
		if constexpr (sizeof...(Args) != 0)
			return Flags<E, D>(flag) | combine(args...);
		else
			return Flags<E, D>(flag);
	}

	template<typename E>
	using Flags32 = Flags<E, u32>;
	template<typename E>
	using Flags64 = Flags<E, u64>;
}