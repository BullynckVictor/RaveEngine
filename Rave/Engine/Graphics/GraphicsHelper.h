#pragma once
#include "Engine/Graphics/WindowRenderer.h"
#include "Engine/Utility/Error.h"
#include "Engine/Core/Engine.h"

namespace rv
{
	template<DrawableConcept D>
	Result WindowRenderer::AddDrawable(D& drawable)
	{
		rv_result;
		rif_assert(engine);
		if constexpr (DrawableStaticPipeline<D>)
		{
			if (initializedPipelines.contains(typeid(D).hash_code()))
				return result;

			initializedPipelines.insert(typeid(D).hash_code());
			for (u32 i = 0; i < D::nPipelines; ++i)
			{
				rv::FullPipeline* pipeline;
				rv::PipelineLayoutDescriptor layout;
				drawable.DescribePipeline(engine->graphics, layout, i);
				layout.Rehash();
				GetPipeline(pipeline, layout);

				DrawableRecorder recorder;
				recorder.recordFunction = D::RecordCommand;
				recorder.drawable = drawable.id();
				recorder.pipeline = pipeline;
				recorder.commandIndex = (u32)drawCommands[0].size();
				recorders.push_back(recorder);

				for (size_t i = 0; i < drawCommands.size(); ++i)
				{
					CommandBuffer draw;
					rv_rif(CommandBuffer::Create(draw, engine->graphics.device, drawPool));
					rv_rif(Record(recorder, draw, i));
					drawCommands[i].push_back(std::move(draw));
				}
			}
		}
		else
		{

		}
		return result;
	}
}