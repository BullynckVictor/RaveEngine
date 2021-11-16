#pragma once
#include "Engine/Graphics/Renderer.h"
#include "Engine/Graphics/SwapChain.h"
#include "Engine/Core/Window.h"

namespace rv
{
	class WindowRenderer : public Renderer
	{
	public:
		WindowRenderer() = default;

		static Result Create(
			WindowRenderer& renderer, 
			const Instance& instance, 
			const Device& device, 
			const WindowDescriptor& window = {},
			const SwapChainPreferences& preferences = {}
		);

		void EndFrame() override;

		Window window;

	private:
		SwapChain swap;
	};
}