#include "Engine/Graphics/WindowRenderer.h"
#include "Engine/Graphics/GraphicsHelper.h"
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

rv::WindowRenderer::~WindowRenderer()
{
	if (engine)
		engine->graphics.device.Wait();
}

rv::Result rv::WindowRenderer::Create(WindowRenderer& renderer, Engine& engine, const FColor& background, const WindowDescriptor& window, const SwapChainPreferences& preferences)
{
	rv_result;

	renderer.SetEngine(engine);
	renderer.background = background;
	renderer.swapPreferences = preferences;

	rv_rif(CommandPool::CreateGraphics(renderer.drawPool, engine.graphics.device, true));
	check_debug_static();

	rv_rif(SwapChain::SetFullScreenFunctions(engine.graphics.instance));

	rv_rif(Window::Create(renderer.window, window));
	rv_rif(renderer.Resize());

	renderer.frames.resize(2);
	rv_rif(Frame::Create(renderer.frames[0], engine.graphics.device, renderer.swap));
	rv_rif(Frame::Create(renderer.frames[1], engine.graphics.device, renderer.swap));
	renderer.window.Resized();
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

rv::Result rv::WindowRenderer::SetVSync(bool vsync)
{
	if (vsync != (swap.presentMode == VK_PRESENT_MODE_FIFO_KHR))
	{
		swapPreferences.presentMode = vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
		return Resize();
	}
	return success;
}

bool rv::WindowRenderer::FullScreen() const
{
	return window.FullScreen();
}

rv::Result rv::WindowRenderer::SetFullScreen(bool fullscreen)
{
	rv_result;
	rv_rif(window.SetFullScreen(fullscreen));
	return swap.SetFullScreen(fullscreen);
}

rv::Result rv::WindowRenderer::ToggleFullScreen()
{
	bool fullscreen = !window.FullScreen();
	return SetFullScreen(fullscreen);
}

rv::Result rv::WindowRenderer::CreateShape(Shape& shape, const HeapBuffer<Vertex2>& vertices, const HeapBuffer<u16>& indices)
{
	rv_result;
	rv_rif(engine->graphics.CreateShape(shape, vertices, indices));
	rv_rif(GraphicsHelper::InitDrawable(*this, shape));
	return result;
}

rv::Result rv::WindowRenderer::CreateShape(Shape& shape, HeapBuffer<Vertex2>&& vertices, HeapBuffer<u16>&& indices)
{
	rv_result;
	rv_rif(engine->graphics.CreateShape(shape, std::move(vertices), std::move(indices)));
	rv_rif(GraphicsHelper::InitDrawable(*this, shape));
	return result;
}

rv::Result rv::WindowRenderer::Render()
{
	if (window.Minimized() || window.Size().height <= 0)
		return success;
	Result result;

	if (window.Resized())
		rv_rif(Resize());

	u32 image;
	u32 currentFrame = nextFrame;
	bool resized;
	nextFrame = (currentFrame + 1) % (u32)frames.size();
	rv_rif(frames[currentFrame].Start(image, resized));

	if (resized)
		return Resize();

	for (auto it = drawCommands[image].begin(); it < drawCommands[image].end() - 1; ++it)
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
		frames[currentFrame].RenderLast(drawCommands[image].back());
	Result r = frames[currentFrame].End(resized);
	check_debug();
	if (resized)
		return Resize();
	if (r.severity() > result.severity())
		return r;
	return result;
}

rv::Result rv::WindowRenderer::Wait() const
{
	return Frame::Wait(engine->graphics.device, frames);
}

rv::Result rv::WindowRenderer::Resize()
{
	if (window.Size().height <= 0)
		return success;

	rv_result;
	rv_rif(Wait());

	VkFormat oldFormat = swap.format.format;
	bool resized = swap.swap;

	rv_rif(SwapChain::Create(swap, engine->graphics.instance, engine->graphics.device, window, swapPreferences));
	check_debug();

	if (!resized || oldFormat != swap.format.format)
	{
		RenderPassDescriptor color;
		color.AddSubpass();
		color.AddColorAttachment(0, swap.format.format);
		color.AddColorDependency();
		rv_rif(RenderPass::Create(colorPass, engine->graphics.device, color));
		check_debug();
		RenderPassDescriptor clear;
		clear.AddSubpass();
		clear.AddColorAttachment(0, swap.format.format, VK_ATTACHMENT_LOAD_OP_CLEAR);
		clear.AddColorDependency();
		rv_rif(RenderPass::Create(clearPass, engine->graphics.device, clear));
		check_debug();
	}

	frameBuffers.resize(swap.images.size());
	for (size_t i = 0; i < frameBuffers.size(); ++i)
		rv_rif(FrameBuffer::Create(frameBuffers[i], engine->graphics.device, colorPass, window.Size(), swap.views[i]));
	check_debug();

	drawCommands.resize(frameBuffers.size());
	for (size_t i = 0; i < drawCommands.size(); ++i)
	{
		CommandBuffer& clear = resized ? drawCommands[i][0] : drawCommands[i].emplace_back();
		if (!resized)
			rv_rif(CommandBuffer::Create(clear, engine->graphics.device, drawPool));
		else
			rv_rif(clear.Reset());
		rv_rif(clear.Begin());
		clear.StartRenderPass(clearPass, frameBuffers[i], 0, window.Size(), background);
		clear.EndRenderPass();
		rv_rif(clear.End());
	}

	for (auto& pipeline : pipelines)
	{
		pipeline.second.layout.SetSize(window.Size());
		pipeline.second.layout.pass = colorPass.pass;
		rv_rif(Pipeline::Create(pipeline.second.pipeline, engine->graphics.device, pipeline.second.layout));
	}

	for (const auto& recorder : recorders)
	{
		for (size_t i = 0; i < drawCommands.size(); ++i)
		{
			CommandBuffer& draw = drawCommands[i][recorder.commandIndex];
			rv_rif(Record(recorder, draw, i));
		}
	}
	return result;
}

rv::Result rv::WindowRenderer::Record(const DrawableRecorder& recorder, CommandBuffer& draw, size_t index)
{
	rv_result;
	rv_rif(draw.Begin());
	draw.StartRenderPass(colorPass, frameBuffers[index], 0, window.Size());
	draw.BindPipeline(recorder.pipeline->pipeline);
	recorder.recordFunction(draw, *recorder.data);
	draw.EndRenderPass();
	return draw.End();
}
