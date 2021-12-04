#pragma once
#include "Engine/Graphics/CommandBuffer.h"

namespace rv
{
	template<typename T>
	concept DrawableConcept = requires(T drawable, CommandBuffer cmd, u32 i, FullPipeline* pipeline)
	{
		drawable.RecordCommand(cmd, i);
		drawable.SetPipeline(pipeline);
		drawable.Initialised();
	};
}