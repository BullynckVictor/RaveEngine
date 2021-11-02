#pragma once
#include "Engine/Utility/Hash.h"

namespace rv
{
	template<typename H>
	struct BasicIdentifier
	{
	public:
		constexpr BasicIdentifier() : m_hash(0), m_name(nullptr) {}
		constexpr BasicIdentifier(const char* name) : m_hash(hash<H>(name)), m_name(name) {}
		constexpr BasicIdentifier(const BasicIdentifier& rhs) : m_hash(rhs.m_hash), m_name(rhs.m_name) {}
		constexpr BasicIdentifier(BasicIdentifier&& rhs) : m_hash(rhs.m_hash), m_name(rhs.m_name) { rhs.m_hash = 0; rhs.m_name = nullptr; }

		constexpr BasicIdentifier& operator= (const BasicIdentifier& rhs) { m_hash = rhs.m_hash; m_name = rhs.m_name; return *this; }
		constexpr BasicIdentifier& operator= (BasicIdentifier&& rhs) { m_hash = rhs.m_hash; m_name = rhs.m_name; rhs.m_hash = 0; rhs.m_name = nullptr; return *this; }
		constexpr BasicIdentifier& operator= (const char* name) { m_hash = hash<H>(name); m_name = name; return *this; }

		constexpr bool operator== (const BasicIdentifier& rhs) const { return m_hash == rhs.m_hash; }
		constexpr bool operator!= (const BasicIdentifier& rhs) const { return m_hash != rhs.m_hash; }
		constexpr bool operator<= (const BasicIdentifier& rhs) const { return m_hash <= rhs.m_hash; }
		constexpr bool operator>= (const BasicIdentifier& rhs) const { return m_hash >= rhs.m_hash; }
		constexpr bool operator<  (const BasicIdentifier& rhs) const { return m_hash <  rhs.m_hash; }
		constexpr bool operator>  (const BasicIdentifier& rhs) const { return m_hash >  rhs.m_hash; }

		constexpr bool valid() const { return m_name; }
		constexpr bool invalid() const { return !m_name; }

		constexpr H hash() const { return m_hash; }
		constexpr const char* name() const { return m_name; }

	private:
		H m_hash;
		const char* m_name;
	};

	typedef BasicIdentifier<size_t> Identifier;
	typedef BasicIdentifier<u32> Identifier32;
	typedef BasicIdentifier<u64> Identifier64;
}