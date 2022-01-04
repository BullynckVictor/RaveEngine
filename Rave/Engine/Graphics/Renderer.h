#pragma once
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Graphics/FrameBuffer.h"
#include "Engine/Drawable/Drawable.h"
#include "Engine/Utility/Multimap.h"
#include <set>

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

		template<DrawableRendererData D>
		D::RendererData& GetData(const D& drawable) { return drawableData.get<D::RendererData>(drawable.id()); }
		template<DrawableRendererData D>
		D::RendererData& GetData(D drawable) { return drawableData.get<D::RendererData>(drawable.id()); }
		template<DrawableImageData D>
		D::ImageData& GetImageData(const D& drawable, u32 image) { return drawableData.get<D::ImageData>(drawable.id(), image); }
		template<DrawableImageData D>
		D::ImageData& GetImageDataInterpreted(Drawable drawable, u32 image) { return drawableData.get<D::ImageData>(drawable.id(), image); }

	protected:
		FullPipeline* GetCachedPipeline(const PipelineLayoutDescriptor& layout);
		Result PrepNewPipeline(FullPipeline*& pipeline, const PipelineLayoutDescriptor& layout);

	protected:
		Engine* engine = nullptr;
		std::map<PipelineLayoutDescriptor, FullPipeline> pipelines;
		std::set<size_t> initializedPipelines;

		MultiMap drawableData;
	};
}