#pragma once
#include "Engine/Graphics/WindowRenderer.h"
#include "Engine/Utility/Error.h"
#include "Engine/Core/Engine.h"
#include "Engine/Graphics/WindowRenderer.h"

namespace rv
{
	class GraphicsHelper
	{
		template<typename D>
		static void Manage(Graphics& graphics, Drawable<D>& drawable)
		{
			if (!drawable.data)
			{
				D* data = new D();
				drawable.data = data;
				graphics.AddDrawable(data);
			}
		}

		template<DrawableConcept D>
		static Result InitDrawable(WindowRenderer& renderer, D& drawable)
		{
			rv_result;
			rif_assert(renderer.engine);

			if (!renderer.initialisedDrawableTypes.contains(typeid(D).hash_code()))
			{
				for (size_t i = 0; i < D::nPipelines; ++i)
				{
					rv::FullPipeline* pipeline;
					rv::PipelineLayoutDescriptor layout;
					drawable.DescribePipeline(layout, i);
					layout.rehash();
					renderer.GetPipeline(pipeline, layout);

					DrawableRecorder recorder;
					recorder.recordFunction = D::RecordCommand;
					recorder.data = drawable.data;
					recorder.pipeline = pipeline;
					recorder.commandIndex = renderer.drawCommands[0].size();
					renderer.recorders.push_back(recorder);

					for (size_t i = 0; i < renderer.drawCommands.size(); ++i)
					{
						CommandBuffer draw;
						rv_rif(CommandBuffer::Create(draw, renderer.engine->graphics.device, renderer.drawPool));
						rv_rif(renderer.Record(recorder, draw, i));
						renderer.drawCommands[i].push_back(std::move(draw));
					}
				}
			}

			return result;
		}

		friend class WindowRenderer;
		friend class Graphics;
	};
}