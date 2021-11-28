#pragma once
#include <cinttypes>
#include <type_traits>

namespace rv
{
	typedef int8_t				int8;
	typedef int16_t				int16;
	typedef int32_t				int32;
	typedef int64_t				int64;

	typedef uint8_t				uint8;
	typedef uint16_t			uint16;
	typedef uint32_t			uint32;
	typedef uint64_t			uint64;

	typedef int8				i8;
	typedef int16				i16;
	typedef int32				i32;
	typedef int64				i64;

	typedef uint8				u8;
	typedef uint16				u16;
	typedef uint32				u32;
	typedef uint64				u64;

	typedef u8					byte;
	typedef const char*			cstring;
	typedef unsigned char		uchar;
	typedef unsigned short		ushort;
	typedef unsigned int		uint;
	typedef unsigned long		ulong;
	typedef unsigned long long	ulonglong;
	typedef unsigned long long	ull;

	template<typename... T>
	struct all_same : std::false_type { };
	template<>
	struct all_same<> : std::true_type { };
	template<typename T>
	struct all_same<T> : std::true_type { };
	template<typename T, typename... Ts>
	struct all_same<T, T, Ts...> : all_same<T, Ts...> { };
}