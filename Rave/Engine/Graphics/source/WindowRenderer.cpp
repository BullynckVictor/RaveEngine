#include "Engine/Graphics/WindowRenderer.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"
#include "Engine/Core/Logger.h"

rv::Result rv::WindowRenderer::Create(WindowRenderer& renderer, const Instance& instance, const Device& device, const WindowDescriptor& window, const SwapChainPreferences& preferences)
{
	rv_result;

	rv_rif(Window::Create(renderer.window, window));
	return SwapChain::Create(renderer.swap, instance, device, renderer.window, preferences);
}

void rv::WindowRenderer::EndFrame()
{
	
}
