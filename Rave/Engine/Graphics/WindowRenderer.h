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
			Engine& engine,
			const WindowDescriptor& window = {},
			const SwapChainPreferences& preferences = {}
		);

		Result GetPipeline(FullPipeline*& pipeline, const PipelineLayoutDescriptor& layout);
		Result AddPipeline(const PipelineLayoutDescriptor& layout);

		void EndFrame() override;

		Window window;

	private:
		SwapChain swap;
		RenderPass clear;
	};
}