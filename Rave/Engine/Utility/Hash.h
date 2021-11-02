#pragma once
#include "Engine/Core/CompileTimeInfo.h"
#include <string_view>

namespace rv
{
	template<typename... Args>
	static constexpr size_t combine_hash(size_t& a, size_t b)
	{
		a ^= b + 0x9e3779b9 + (a << 6) + (a >> 2);
		return a;
	}

	namespace hash_alg
	{
		template<size_t S>
		struct Fnv1a_constants;

		template<>
		struct Fnv1a_constants<4>
		{
			static constexpr size_t prime = (size_t)0x01000193;
			static constexpr size_t offset = (size_t)0x811c9dc5;
		};

		template<>
		struct Fnv1a_constants<8>
		{
			static constexpr size_t prime = (size_t)0x00000100000001b3;
			static constexpr size_t offset = (size_t)0xcbf29ce484222325;
		};

		static constexpr Fnv1a_constants<cti.architecture.pointer_size> fnv1a_constants;

		template<typename T, size_t D>
		static constexpr size_t hash_element(const T* data, size_t hash)
		{
			if constexpr (D < sizeof(T))
			{
				hash = hash ^ ((*data >> (D * 8)) & 0xFF);
				hash = hash * fnv1a_constants.prime;
				hash = hash_element<T, D + 1>(data, hash);
			}
			return hash;
		}

		template<typename T>
		static constexpr size_t fnv1a(const T* data, size_t s)
		{
			size_t hash = fnv1a_constants.offset;

			for (const T* b = data; b < data + s; ++b)
				hash = hash_element<T, 0>(b, hash);

			return hash;
		}

		template<typename T>
		static constexpr bool is_string()
		{
			return
				std::is_same_v<T, std::string> ||
				std::is_same_v<T, std::wstring> ||
				std::is_same_v<T, std::string_view> ||
				std::is_same_v<T, std::wstring_view>;
		}

		template<typename T>
		static constexpr bool is_cstring()
		{
			return
				std::is_same_v<T, const char*> ||
				std::is_same_v<T, const wchar_t*>;
		}

		template<typename T, typename... Args>
		static constexpr size_t hash(const T& value, const Args&... args)
		{
			size_t h;

			if constexpr (is_string<T>())
				h = fnv1a(value.data(), value.size());

			else if constexpr (is_cstring<T>())
				h = fnv1a(value, strlen(value));

			else if constexpr (std::is_integral_v<T> || std::is_enum_v<T>)
				h = fnv1a(&value, 1);

			else if constexpr (std::is_floating_point_v<T>)
			{
				T v = value == (T)0 ? (T)0 : value;
				h = fnv1a(reinterpret_cast<const byte*>(&v), sizeof(T));
			}

			else
				h = fnv1a(reinterpret_cast<const byte*>(&value), sizeof(T));

			if constexpr (sizeof...(Args) != 0)
				h = combine_hash(h, hash(args...));

			return h;
		}
	}

	template<typename... Args>
	static constexpr size_t hash(const Args&... args)
	{
		if constexpr (sizeof...(Args) != 0)
			return hash_alg::hash(args...);
		else
			return hash_alg::fnv1a(nullptr, 0);
	}
}