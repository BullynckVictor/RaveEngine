#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Utility/Multimap.h"
#include "Engine/Graphics/Graphics.h"
#include "Engine/Graphics/WindowRenderer.h"

namespace rv
{
	class Engine
	{
	public:
		Engine() = default;
		static Result Create(Engine& engine, const GraphicsInfo& info = {});
		static Result Create(Engine& engine, const char* appName, const u32 version = VK_MAKE_VERSION(1, 0, 0));
		static Result Create(Engine& engine, const std::vector<std::reference_wrapper<const Surface>>& surfaces);
		static Result Create(Engine& engine, const std::vector<std::reference_wrapper<const Window>>& windows);
		static Result Create(Engine& engine, const Surface& surface);
		static Result Create(Engine& engine, const Window& window);

		Graphics graphics;
	};
}