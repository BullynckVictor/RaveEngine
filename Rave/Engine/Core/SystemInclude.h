#pragma once
#include "Engine/Core/CompileTimeInfo.h"

#ifdef RV_PLATFORM_WINDOWS
#	define NOMINMAX
#	include <Windows.h>
#	undef NOMINMAX

namespace rv
{
	namespace System
	{
		static const char* GetEnv(const char* var)
		{
			static char buffer[65535];
			if (GetEnvironmentVariable(var, buffer, 65535))
				return buffer;
			return nullptr;
		}
	}
}

#endif