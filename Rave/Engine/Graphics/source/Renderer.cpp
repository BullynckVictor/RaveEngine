#include "Engine/Graphics/Renderer.h"
#include "Engine/Core/Engine.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"

rv::Renderer::Renderer(Engine& e)
	:
	engine(&e)
{
}

rv::Result rv::Renderer::Render()
{
	return success;
}

void rv::Renderer::SetEngine(Engine& e)
{
	engine = &e;
}

rv::FullPipeline* rv::Renderer::GetCachedPipeline(const PipelineLayoutDescriptor& layout)
{
	auto it = pipelines.find(layout);
	return (it == pipelines.end()) ? nullptr : &it->second;
}

rv::Result rv::Renderer::PrepNewPipeline(FullPipeline*& pipeline, const PipelineLayoutDescriptor& layout)
{
	rv_result;

	pipeline = &pipelines[layout];
	pipeline->layout.SetToDescriptor(layout);

	for (const auto shader : layout.shaders)
	{
		Shader* s;
		rv_rif(engine->graphics.GetShader(s, shader));
		pipeline->layout.AddShader(*s);
	}

//	RenderPass* pass = GetRenderPass(layout.renderpass);
//	rif_assert(pass);
//	pipeline->layout.AddRenderPass(*pass, layout.subpass);

	return result;
}
