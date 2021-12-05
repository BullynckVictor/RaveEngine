#pragma once
#include "Engine/Drawable/Drawable.h"

namespace rv
{
	struct ShapeData : public DrawableData
	{
		void RecordCommand(CommandBuffer& command, u32 index) const override;

		static const PipelineLayoutDescriptor& GetLayout();
		static bool Initialised();

		static FullPipeline* pipeline;
		static PipelineLayoutDescriptor layout;
	};

	class Shape : public Drawable<ShapeData>
	{
	public:


	private:
	};
}