#pragma once
#include "Engine/Core/CompileTimeInfo.h"

#ifdef RV_PLATFORM_WINDOWS
#	define NOMINMAX
#	include <Windows.h>
#	undef NOMINMAX
#endif