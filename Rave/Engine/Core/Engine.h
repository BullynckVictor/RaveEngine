#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Graphics/GraphicsEngine.h"

namespace rv
{
	class Engine
	{
	public:
		Engine() = default;
		static Result Create(Engine& engine, const GraphicsEngineInfo& info = {});
		static Result Create(Engine& engine, const char* appName, const u32 version = VK_MAKE_VERSION(1, 0, 0));

		GraphicsEngine graphics;
	};
}