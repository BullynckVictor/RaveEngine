#pragma once
#include "Engine/Drawable/Drawable.h"

namespace rv
{
	struct ShapeData : public DrawableData
	{
		MappedVertexBuffer<Vertex2> vertices;
	};

	class Shape : public Drawable<ShapeData>
	{
	public:
		static Result Create(Shape& shape, const MemoryAllocator& allocator, const std::vector<Vertex2>& vertices);
		static Result Create(Shape& shape, const MemoryAllocator& allocator, std::vector<Vertex2>&& vertices);

		static void RecordCommand(CommandBuffer& draw, const DrawableData& data);
		static void DescribePipeline(PipelineLayoutDescriptor& layout, size_t index);
		static constexpr size_t nPipelines = 1;
	};
}