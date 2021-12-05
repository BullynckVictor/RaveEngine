#include "Engine/Drawable/Shape.h"

rv::FullPipeline* rv::ShapeData::pipeline = nullptr;
rv::PipelineLayoutDescriptor rv::ShapeData::layout;

void rv::ShapeData::RecordCommand(CommandBuffer& command, u32 index) const
{
	command.BindPipeline(pipeline->pipeline);
	command.Draw(3);
}

const rv::PipelineLayoutDescriptor& rv::ShapeData::GetLayout()
{
	if (!Initialised())
	{
		layout.shaders = {
			"triangle.vert",
			"triangle.frag"
		};
		layout.rehash();
	}
	return layout;
}

bool rv::ShapeData::Initialised()
{
	return pipeline;
}
