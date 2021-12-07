#include "Engine/Drawable/Shape.h"

rv::FullPipeline* rv::ShapeData::pipeline = nullptr;
rv::PipelineLayoutDescriptor rv::ShapeData::layout;

void rv::ShapeData::RecordCommand(CommandBuffer& command, u32 index) const
{
	command.BindPipeline(pipeline->pipeline);
	command.BindVertexBuffer(vertices);
	command.Draw(vertices.Size());
}

const rv::PipelineLayoutDescriptor& rv::ShapeData::GetLayout()
{
	if (!Initialised())
	{
		layout.shaders = {
			"triangle.vert",
			"triangle.frag"
		};
		layout.vertex.Set<Vertex2>();
		layout.rehash();
	}
	return layout;
}

bool rv::ShapeData::Initialised()
{
	return pipeline;
}

rv::Result rv::ShapeData::Create(ShapeData& shape, const MemoryAllocator& allocator, const std::vector<Vertex2>& vertices)
{
	return MappedVertexBuffer<Vertex2>::Create(shape.vertices, allocator, vertices);
}

rv::Result rv::ShapeData::Create(ShapeData& shape, const MemoryAllocator& allocator, std::vector<Vertex2>&& vertices)
{
	return MappedVertexBuffer<Vertex2>::Create(shape.vertices, allocator, std::move(vertices));
}

rv::MappedVertexBuffer<rv::Vertex2>& rv::Shape::Vertices()
{
	return data->vertices;
}

const rv::MappedVertexBuffer<rv::Vertex2>& rv::Shape::Vertices() const
{
	return data->vertices;
}
