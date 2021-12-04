#include "Engine/Graphics/WindowRenderer.h"
#include "Engine/Graphics/WindowRendererHelper.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"
#include "Engine/Core/Logger.h"
#include "Engine/Core/Engine.h"

#ifdef RV_DEBUG
#	define check_debug()		rv_rif(engine->graphics.debug.Check())
#	define check_debug_static()	rv_rif(renderer.engine->graphics.debug.Check())
#else
#	define check_debug()
#	define check_debug_static()
#endif

rv::Result rv::WindowRenderer::Create(WindowRenderer& renderer, Engine& engine, const FColor& background, const WindowDescriptor& window, const SwapChainPreferences& preferences)
{
	rv_result;

	renderer.SetEngine(engine);
	renderer.background = background;

	rv_rif(Window::Create(renderer.window, window));
	rv_rif(SwapChain::Create(renderer.swap, engine.graphics.instance, engine.graphics.device, renderer.window, preferences));
	check_debug_static();

	RenderPassDescriptor color;
	color.AddSubpass();
	color.AddColorAttachment(0, renderer.swap.format.format);
	color.AddColorDependency();
	rv_rif(RenderPass::Create(renderer.colorPass, engine.graphics.device, color));
	check_debug_static();
//	rv_rif(renderer.AddRenderPass("color", color));
	RenderPassDescriptor clear;
	clear.AddSubpass();
	clear.AddColorAttachment(0, renderer.swap.format.format, VK_ATTACHMENT_LOAD_OP_CLEAR);
	clear.AddColorDependency();
	rv_rif(RenderPass::Create(renderer.clearPass, engine.graphics.device, clear));
	check_debug_static();

	renderer.frameBuffers.resize(renderer.swap.images.size());
	for (size_t i = 0; i < renderer.frameBuffers.size(); ++i)
		rv_rif(FrameBuffer::Create(renderer.frameBuffers[i], engine.graphics.device, renderer.colorPass, renderer.window.Size(), renderer.swap.views[i]));
	check_debug_static();

	renderer.drawCommands.resize(renderer.frameBuffers.size());

	rv_rif(CommandPool::CreateGraphics(renderer.drawPool, engine.graphics.device));
	check_debug_static();

	for (size_t i = 0; i < renderer.drawCommands.size(); ++i)
	{
		CommandBuffer& clear = renderer.drawCommands[i].emplace_back();
		rv_rif(CommandBuffer::Create(clear, engine.graphics.device, renderer.drawPool));
		rv_rif(clear.Begin());
		clear.StartRenderPass(renderer.clearPass, renderer.frameBuffers[i], 0, renderer.window.Size(), background);
		clear.EndRenderPass();
		rv_rif(clear.End());
	}

	renderer.frames.resize(2);
	rv_rif(Frame::Create(renderer.frames[0], engine.graphics.device, renderer.swap));
	rv_rif(Frame::Create(renderer.frames[1], engine.graphics.device, renderer.swap));
	return result;
}

rv::Result rv::WindowRenderer::Create(WindowRenderer& renderer, Engine& engine, const std::string& title, uint width, uint height, const FColor& background, bool vsync, bool resize)
{
	WindowDescriptor window;
	SwapChainPreferences swap(vsync);

	window.title = title;
	window.size = { width, height };
	window.resize = resize;

	return Create(renderer, engine, background, window, swap);
}

rv::Result rv::WindowRenderer::GetPipeline(FullPipeline*& pipeline, const PipelineLayoutDescriptor& layout)
{
	rv_result;
	rif_assert(engine);

	pipeline = GetCachedPipeline(layout);
	if (pipeline)
		return success;
	
	result = PrepNewPipeline(pipeline, layout);
	if (result.failed())
	{
		pipeline = nullptr;
		return result;
	}

	pipeline->layout.SetSize(window.Size());
	pipeline->layout.pass = colorPass.pass;

	result = FullPipeline::Create(*pipeline, engine->graphics.device);
	if (result.failed())
		pipeline = nullptr;

	check_debug();

	return result;
}

rv::Result rv::WindowRenderer::AddPipeline(const PipelineLayoutDescriptor& layout)
{
	FullPipeline* pipeline;
	return GetPipeline(pipeline, layout);
}

rv::Result rv::WindowRenderer::CreateShape(Shape& shape)
{
	rv_result;
	WindowRendererHelper::InitDrawable(*this, shape);
	return result;
}

rv::Result rv::WindowRenderer::Render()
{
	Result result;
	ResultValue<u32> image;
	u32 currentFrame = nextFrame;
	nextFrame = (currentFrame + 1) % (u32)frames.size();
	image = frames[currentFrame].Start();
	if (image.failed())
		return image;
	for (auto it = drawCommands[image.value].begin(); it < drawCommands[image.value].end() - 1; ++it)
	{
		const CommandBuffer& draw = *it;
		Result r = frames[currentFrame].Render(draw);
		if (r.severity() > result.severity())
		{
			result = r;
			if (result.fatal())
				return result;
		}
	}
	if (!drawCommands.empty())
		frames[currentFrame].RenderLast(drawCommands[image.value].back());
	Result r = frames[currentFrame].End();
	if (r.severity() > result.severity())
		return r;
	return result;
}