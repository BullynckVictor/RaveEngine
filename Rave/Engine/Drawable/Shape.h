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

		static Result Create(ShapeData& shape, const MemoryAllocator& allocator, const std::vector<Vertex2>& vertices);
		static Result Create(ShapeData& shape, const MemoryAllocator& allocator, std::vector<Vertex2>&& vertices);

		MappedVertexBuffer<Vertex2> vertices;
	};

	class Shape : public Drawable<ShapeData>
	{
	public:
		MappedVertexBuffer<Vertex2>& Vertices();
		const MappedVertexBuffer<Vertex2>& Vertices() const;
	};
}