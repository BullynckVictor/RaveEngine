#include "Engine/Drawable/Shape.h"
#include "Engine/Core/Logger.h"
#include "Engine/Utility/Error.h"

rv::Result rv::Shape::Create(Shape& shape, const StagingBufferManager& manager, const HeapBuffer<Vertex2>& vertices, const HeapBuffer<u16>& indices)
{
	rv_result;
	rv_rif(VertexBuffer::Create(shape.data->vertexBuffer, shape.data->vertices, manager, vertices.data(), vertices.size() * sizeof(Vertex2)));
	rv_rif(IndexBuffer::Create(shape.data->indexBuffer, shape.data->indices, manager, indices.data(), indices.size() * sizeof(u16), VK_INDEX_TYPE_UINT16));
	shape.data->vertices.data = vertices;
	shape.data->indices.data = indices;
	return result;
}

rv::Result rv::Shape::Create(Shape& shape, const StagingBufferManager& manager, HeapBuffer<Vertex2>&& vertices, HeapBuffer<u16>&& indices)
{
	rv_result;
	rv_rif(VertexBuffer::Create(shape.data->vertexBuffer, shape.data->vertices, manager, vertices.data(), vertices.size() * sizeof(Vertex2)));
	rv_rif(IndexBuffer::Create(shape.data->indexBuffer, shape.data->indices, manager, indices.data(), indices.size() * sizeof(u16), VK_INDEX_TYPE_UINT16));
	shape.data->vertices.data = std::move(vertices);
	shape.data->indices.data = std::move(indices);
	return result;
}

void rv::Shape::RecordCommand(CommandBuffer& draw, const DrawableData& data)
{
	const ShapeData& shape = reinterpret_cast<const ShapeData&>(data);
	draw.BindVertexBuffer(shape.vertexBuffer);
	draw.BindIndexBuffer(shape.indexBuffer);
	draw.DrawIndexed(shape.indices.Size());
}

void rv::Shape::DescribePipeline(PipelineLayoutDescriptor& layout, size_t index)
{
	layout.shaders = {
		"triangle.vert",
		"triangle.frag"
	};
	layout.cullMode = VK_CULL_MODE_NONE;
	layout.clockwise = true;
	layout.vertex.Set<Vertex2>();
	layout.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}
