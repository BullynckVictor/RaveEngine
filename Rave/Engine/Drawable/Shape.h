#pragma once
#include "Engine/Drawable/Drawable.h"

namespace rv
{
	class Shape
	{
	public:
		void RecordCommand(CommandBuffer& command, u32 index);
		
		static const PipelineLayoutDescriptor& GetLayout();
		static bool Initialised();
		static void SetPipeline(FullPipeline* pipeline);

	private:
		static FullPipeline* pipeline;
		static PipelineLayoutDescriptor layout;
	};
}