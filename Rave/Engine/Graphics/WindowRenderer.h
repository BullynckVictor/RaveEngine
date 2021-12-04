#pragma once
#include "Engine/Graphics/Renderer.h"
#include "Engine/Graphics/SwapChain.h"
#include "Engine/Graphics/CommandBuffer.h"
#include "Engine/Graphics/Frame.h"
#include "Engine/Core/Window.h"
#include "Engine/Drawable/Shape.h"

namespace rv
{
	class WindowRenderer : public Renderer
	{
	public:
		WindowRenderer() = default;

		static Result Create(
			WindowRenderer& renderer, 
			Engine& engine,
			const FColor& color = FColors::White,
			const WindowDescriptor& window = {},
			const SwapChainPreferences& preferences = {}
		);
		static Result Create(
			WindowRenderer& renderer,
			Engine& engine,
			const std::string& title, 
			uint width, 
			uint height, 
			const FColor& background = FColors::White,
			bool vsync = true, 
			bool resize = false
		);

		Result GetPipeline(FullPipeline*& pipeline, const PipelineLayoutDescriptor& layout);
		Result AddPipeline(const PipelineLayoutDescriptor& layout);

		Result CreateShape(Shape& shape);

		Result Render() override;

		Window window;

	private:
		SwapChain swap;
		RenderPass clearPass;
		RenderPass colorPass;
		std::vector<FrameBuffer> frameBuffers;
		std::vector<std::vector<CommandBuffer>> drawCommands;
		CommandPool drawPool;
		FColor background;
		std::vector<Frame> frames;
		u32 nextFrame = 0;

		friend class WindowRendererHelper;
	};
}