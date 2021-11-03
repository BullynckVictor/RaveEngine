#pragma once
#include "Engine/Utility/Types.h"

#	ifdef _WIN32
#		define RV_PLATFORM_WINDOWS 1
#		define RV_PLATFORM_SUPPORTED 1
#	endif

#	if defined(unix) || defined(__unix__) || defined(__unix)
#		define RV_PLATFORM_LINUX 1
#		ifdef __APPLE__
#			define RV_PLATFORM_APPLE 1
#		else
#			define RV_PLATFORM_PURE_LINUX 1
#		endif
#	endif

#	ifdef NDEBUG
#		define RV_RELEASE 1
#	else
#		define RV_DEBUG 1
#	endif

#	ifdef RV_DEBUG
#		define rv_debug_only(code) code
#else
#		define rv_debug_only(code)
#	endif

#ifndef RV_PLATFORM_SUPPORTED
#	error Platform not supported
#endif

namespace rv
{
	static constexpr struct CompileTimeInfo
	{
#		ifdef RV_PLATFORM_WINDOWS
			static constexpr struct Platform
			{
				static constexpr bool windows = true;
				static constexpr bool linux = false;
				static constexpr bool purelinux = false;
				static constexpr bool macOS = false;
			} platform {};
#		endif
#		ifdef RV_PLATFORM_APPLE
			static constexpr struct Platform
			{
				static constexpr bool windows = false;
				static constexpr bool linux = true;
				static constexpr bool purelinux = false;
				static constexpr bool macOS = true;
			} platform {};
#		endif
#		ifdef RV_PLATFORM_PURE_LINUX
			static constexpr struct Platform
			{
				static constexpr bool windows = false;
				static constexpr bool linux = true;
				static constexpr bool purelinux = true;
				static constexpr bool macOS = false;
			} platform{};
#		endif

		static constexpr struct Architecture
		{
			static constexpr bool x32 = sizeof(void*) == sizeof(u32);
			static constexpr bool x64 = sizeof(void*) == sizeof(u64);
			static constexpr size_t pointer_size = sizeof(void*);
		} architecture{};

#		ifdef RV_RELEASE
		static constexpr struct Build
		{
			static constexpr bool debug = false;
			static constexpr bool release = true;
		} build{};
#		endif
#		ifdef RV_DEBUG
		static constexpr struct Build
		{
			static constexpr bool debug = true;
			static constexpr bool release = false;
		} build{};
#		endif

	} cti{};
}