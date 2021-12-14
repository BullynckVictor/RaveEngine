#pragma once
#include "Engine/Drawable/Drawable.h"

namespace rv
{
	struct ShapeData : public DrawableData
	{
		VertexBuffer vertexBuffer;
		IndexBuffer indexBuffer;
		MappedStagingBuffer<Vertex2> vertices;
		MappedStagingBuffer<u16> indices;
	};

	class Shape : public Drawable<ShapeData>
	{
	public:
		static Result Create(Shape& shape, const StagingBufferManager& manager, const HeapBuffer<Vertex2>& vertices, const HeapBuffer<u16>& indices);
		static Result Create(Shape& shape, const StagingBufferManager& manager, HeapBuffer<Vertex2>&& vertices, HeapBuffer<u16>&& indices);

		static void RecordCommand(CommandBuffer& draw, const DrawableData& data);
		static void DescribePipeline(PipelineLayoutDescriptor& layout, size_t index);
		static constexpr size_t nPipelines = 1;
	};
}