#pragma once
#include "Engine/Core/CompileTimeInfo.h"
#include <string_view>
#include <array>

namespace rv
{
	template<typename H>
	static constexpr H combine_hash(H& a, H b)
	{
		a ^= b + (H)0x9e3779b9 + (a << 6) + (a >> 2);
		return a;
	}

	namespace detail
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
		static constexpr Fnv1a_constants<sizeof(u32)> fnv1a_constants_32;
		static constexpr Fnv1a_constants<sizeof(u64)> fnv1a_constants_64;

		template<typename H, typename T, size_t D>
		static constexpr H hash_element(const T* data, H hash)
		{
			if constexpr (D < sizeof(T))
			{
				hash = hash ^ ((*data >> (D * 8)) & 0xFF);
				hash = hash * Fnv1a_constants<sizeof(H)>::prime;
				hash = hash_element<H, T, D + 1>(data, hash);
			}
			return hash;
		}

		template<typename H, typename T>
		static constexpr H fnv1a(const T* data, size_t s)
		{
			H hash = Fnv1a_constants<sizeof(H)>::offset;

			for (const T* b = data; b < data + s; ++b)
				hash = hash_element<H, T, 0>(b, hash);

			return hash;
		}

		template<typename H, typename T>
		static constexpr H hash_string(const T* string)
		{
			H hash = Fnv1a_constants<sizeof(H)>::offset;

			if (string)
				for (const T* b = string; *b != 0; ++b)
					hash = hash_element<H, T, 0>(b, hash);

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

		template<typename T>
		static constexpr bool is_cstring_array()
		{
			return
				std::is_array_v<T> &&
				is_cstring<T>();
		}

		template <typename T, typename = void>
		struct is_iterable : std::false_type {};

		// this gets used only when we can call std::begin() and std::end() on that type
		template <typename T>
		struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
			decltype(std::end(std::declval<T>()))
		>
		> : std::true_type {};

		// Here is a helper:
		template <typename T>
		constexpr bool is_iterable_v = is_iterable<T>::value;

		template<typename H, typename T>
		static constexpr H hash_range(const T& range)
		{
			H h;
			for (const auto& element : range)
				h = combine_hash<H>(h, hash<H>(element));
			return h;
		}

		template<typename H, typename T, typename... Args>
		static constexpr H hash(const T& value, const Args&... args)
		{
			H h;

			if constexpr (std::is_array_v<T>)
				h = fnv1a<H>(value, std::size(value));

			else if constexpr (is_iterable_v<T>)
				h = hash_range<H>(value);

			else if constexpr (is_string<T>())
				h = fnv1a<H>(value.data(), value.size());

			else if constexpr (is_cstring<T>())
				h = hash_string<H>(value);

			else if constexpr (std::is_integral_v<T> || std::is_enum_v<T>)
				h = fnv1a<H>(&value, 1);

			else if constexpr (std::is_floating_point_v<T>)
			{
				T v = value == (T)0 ? (T)0 : value;
				h = fnv1a<H>(reinterpret_cast<const byte*>(&v), sizeof(T));
			}

			else
				h = fnv1a<H>(reinterpret_cast<const byte*>(&value), sizeof(T));

			if constexpr (sizeof...(Args) != 0)
				h = combine_hash<H>(h, hash<H>(args...));

			return h;
		}
	}

	template<typename H, typename... Args>
	static constexpr H hash_t(const Args&... args)
	{
		if constexpr (sizeof...(Args) != 0)
			return detail::hash<H>(args...);
		else
			return detail::fnv1a<H>(nullptr, 0);
	}

	template<typename... Args>
	static constexpr size_t hash(const Args&... args)
	{
		return hash_t<size_t>(args...);
	}

	template<typename... Args>
	static constexpr u32 hash32(const Args&... args)
	{
		return hash_t<u32>(args...);
	}

	template<typename... Args>
	static constexpr u64 hash64(const Args&... args)
	{
		return hash_t<u64>(args...);
	}
}