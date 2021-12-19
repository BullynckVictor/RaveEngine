#pragma once
#include "Engine/Graphics/Renderer.h"
#include "Engine/Graphics/SwapChain.h"
#include "Engine/Graphics/CommandBuffer.h"
#include "Engine/Graphics/Frame.h"
#include "Engine/Core/Window.h"
#include "Engine/Drawable/Shape.h"
#include <set>

namespace rv
{
	class WindowRenderer : public Renderer
	{
	public:
		WindowRenderer() = default;
		~WindowRenderer();

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

		Result CreateShape(Shape& shape, const HeapBuffer<Vertex2>& vertices, const HeapBuffer<u16>& indices);
		Result CreateShape(Shape& shape, HeapBuffer<Vertex2>&& vertices, HeapBuffer<u16>&& indices);

		Result Render() override;
		Result SetVSync(bool vsync);

		bool FullScreen() const;
		Result SetFullScreen(bool fullscreen);
		Result ToggleFullScreen();

		Result Wait() const;

		Window window;

	private:
		Result Resize();
		Result Record(const DrawableRecorder& recorder, CommandBuffer& draw, size_t index);

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
		SwapChainPreferences swapPreferences;
		std::vector<DrawableRecorder> recorders;
		std::set<size_t> initialisedDrawableTypes;

		friend class GraphicsHelper;
	};
}