#include "Engine/Drawable/Shape.h"

rv::FullPipeline* rv::Shape::pipeline = nullptr;
rv::PipelineLayoutDescriptor rv::Shape::layout;

void rv::Shape::RecordCommand(CommandBuffer& command, u32 index)
{
	command.BindPipeline(pipeline->pipeline);
	command.Draw(3);
}

const rv::PipelineLayoutDescriptor& rv::Shape::GetLayout()
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

bool rv::Shape::Initialised()
{
	return pipeline;
}

void rv::Shape::SetPipeline(FullPipeline* p)
{
	pipeline = p;
}