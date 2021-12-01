#include "Engine/Graphics/WindowRenderer.h"
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

rv::Result rv::WindowRenderer::Create(WindowRenderer& renderer, Engine& engine, const WindowDescriptor& window, const SwapChainPreferences& preferences)
{
	rv_result;

	renderer.SetEngine(engine);

	rv_rif(Window::Create(renderer.window, window));
	rv_rif(SwapChain::Create(renderer.swap, engine.graphics.instance, engine.graphics.device, renderer.window, preferences));

	RenderPassDescriptor color;
	color.AddSubpass();
	color.AddColorAttachment(0, renderer.swap.format.format);
	rv_rif(renderer.AddRenderPass("color", color));
	RenderPassDescriptor clear;
	clear.AddSubpass();
	clear.AddColorAttachment(0, renderer.swap.format.format, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE);
	rv_rif(RenderPass::Create(renderer.clear, engine.graphics.device, clear));
	check_debug_static();

	return result;
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

void rv::WindowRenderer::EndFrame()
{
	
}
