#pragma once
#include "Engine/Utility/Result.h"
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

		Result CreateWindowRenderer(WindowRenderer& renderer, const WindowDescriptor& window, const SwapChainPreferences& swap);
		Result CreateWindowRenderer(WindowRenderer& renderer, const std::string& title, uint width, uint height, bool vsync = true, bool resize = false);

		Graphics graphics;
	};
}