#pragma once
#include "Engine/Graphics/WindowRenderer.h"
#include "Engine/Utility/Error.h"
#include "Engine/Core/Engine.h"

namespace rv
{
	class WindowRendererHelper
	{
		template<DrawableDataConcept D>
		static Result InitDrawable(WindowRenderer& renderer, Drawable<D>& managed)
		{
			rv_result;
			rif_assert(renderer.engine);

			if (!managed.data)
			{
				managed.data = new D();
				renderer.engine->graphics.AddDrawable(managed.data);
				renderer.drawables.push_back(managed.data);
			}

			D& drawable = *managed.data;

			if (!drawable.Initialised())
			{
				rv::FullPipeline* pipeline;
				renderer.GetPipeline(pipeline, drawable.GetLayout());
				drawable.pipeline = pipeline;

				for (size_t i = 0; i < renderer.drawCommands.size(); ++i)
				{
					CommandBuffer draw;
					rv_rif(CommandBuffer::Create(draw, renderer.engine->graphics.device, renderer.drawPool));
					rv_rif(draw.Begin());
					draw.StartRenderPass(renderer.colorPass, renderer.frameBuffers[i], 0, renderer.window.Size());
					drawable.RecordCommand(draw, (u32)i);
					draw.EndRenderPass();
					rv_rif(draw.End());
					renderer.drawCommands[i].push_back(std::move(draw));
				}
			}

			return result;
		}

		friend class WindowRenderer;
	};
}