#pragma once
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Graphics/FrameBuffer.h"

namespace rv
{
	class Engine;

	class Renderer
	{
	public:
		Renderer() = default;
		Renderer(Engine& engine);
		virtual ~Renderer() = default;

		virtual Result Render();

		void SetEngine(Engine& engine);

//		Result AddRenderPass(RenderPass*& pass, const Identifier& name, const RenderPassDescriptor& descriptor);
//		Result AddRenderPass(const Identifier& name, const RenderPassDescriptor& descriptor);
//		RenderPass* GetRenderPass(const Identifier& name);

	protected:
		FullPipeline* GetCachedPipeline(const PipelineLayoutDescriptor& layout);
		Result PrepNewPipeline(FullPipeline*& pipeline, const PipelineLayoutDescriptor& layout);

	protected:
		Engine* engine = nullptr;
		std::map<PipelineLayoutDescriptor, FullPipeline> pipelines;
//		std::map<Identifier, RenderPass> passes;
	};
}