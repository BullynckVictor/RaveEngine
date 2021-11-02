#pragma once
#include "Engine/Utility/Types.h"

namespace rv
{
	static constexpr struct CompileTimeInfo
	{
		static constexpr struct Architecture
		{
			static constexpr bool x32 = sizeof(void*) == sizeof(u32);
			static constexpr bool x64 = sizeof(void*) == sizeof(u64);
			static constexpr size_t pointer_size = sizeof(void*);
		} architecture{};
	} cti;
}