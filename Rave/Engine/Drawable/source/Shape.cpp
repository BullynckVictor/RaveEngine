#include "Engine/Drawable/Shape.h"
#include "Engine/Core/Logger.h"

rv::Result rv::Shape::Create(Shape& shape, const MemoryAllocator& allocator, const std::vector<Vertex2>& vertices)
{
	return MappedVertexBuffer<Vertex2>::Create(shape.data->vertices, allocator, vertices);
}

rv::Result rv::Shape::Create(Shape& shape, const MemoryAllocator& allocator, std::vector<Vertex2>&& vertices)
{
	return MappedVertexBuffer<Vertex2>::Create(shape.data->vertices, allocator, std::move(vertices));
}

void rv::Shape::RecordCommand(CommandBuffer& draw, const DrawableData& data)
{
	const ShapeData& shape = reinterpret_cast<const ShapeData&>(data);
	draw.BindVertexBuffer(shape.vertices);
	draw.Draw(shape.vertices.Size());
	rv_log("Recording");
}

void rv::Shape::DescribePipeline(PipelineLayoutDescriptor& layout, size_t index)
{
	layout.shaders = {
		"triangle.vert",
		"triangle.frag"
	};
	layout.cullMode = VK_CULL_MODE_NONE;
	layout.vertex.Set<Vertex2>();
	rv_log("Describing");
}
