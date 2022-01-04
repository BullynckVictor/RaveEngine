#pragma once
#include "Engine/Drawable/Drawable.h"

namespace rv
{
	class Shape : public DrawableType<Shape, RV_DRAWABLE_DATA, RV_DRAWABLE_IMAGE_DATA, RV_DRAWABLE_STATIC_DATA>
	{
	public:
		Shape() = default;

		struct StaticData
		{
			DescriptorSetAllocator::Queue* queue = nullptr;
		};

		struct Data
		{
			VertexBuffer vertexBuffer;
			IndexBuffer indexBuffer;
			MappedStagingBuffer<Vertex2> vertices;
			MappedStagingBuffer<u16> indices;
			FColor color = FColors::White;
		};

		struct ImageData
		{
			DescriptorSet set;
			UniformBuffer buffer;
			StagingBuffer color;
		};

		static Result Create(Shape& shape, Graphics& graphics, const StagingBufferManager& manager, const HeapBuffer<Vertex2>& vertices, const HeapBuffer<u16>& indices, const FColor& color);
		static Result Create(Shape& shape, Graphics& graphics, const StagingBufferManager& manager, HeapBuffer<Vertex2>&& vertices, HeapBuffer<u16>&& indices, const FColor& color);

		static Result InitStaticData(Graphics& graphics, DescriptorSetAllocator& allocator);
		static Result InitImageData(Shape& shape, Graphics& graphics, Renderer& renderer, DescriptorSetAllocator& allocator, const StagingBufferManager& manager, u32 imageCount);
	
		static void RecordCommand(CommandBuffer& draw, Graphics& graphics, Renderer& renderer, const DrawableRecorder& recorder, u32 image);
		static void DescribePipeline(Graphics& graphics, PipelineLayoutDescriptor& layout, u32 index);

		static constexpr u32 nPipelines = 1;
	};
}