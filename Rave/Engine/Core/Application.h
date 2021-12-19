#pragma once
#include "Engine/Utility/Timer.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/InputManager.h"
#include "Engine/Graphics/WindowRenderer.h"

namespace rv
{
	class Application
	{
	public:
		Application();
		Application(const std::string& title, uint width, uint height, const FColor& background = FColors::White, bool vsync = true, bool resize = false);
		Application(const std::string& title, const FColor& background = FColors::White, const WindowDescriptor& window = {}, const SwapChainPreferences& preferences = {});

		void Run();

	protected:
		virtual void Update(const Duration dt) = 0;

	protected:
		Engine engine;
		WindowRenderer renderer;
		InputManager input;
	};
}