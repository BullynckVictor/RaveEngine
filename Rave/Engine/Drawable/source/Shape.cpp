#include "Engine/Drawable/Shape.h"
#include "Engine/Core/Logger.h"
#include "Engine/Utility/Error.h"
#include "Engine/Graphics/Graphics.h"
#include "Engine/Graphics/Renderer.h"


rv::Result rv::Shape::Create(Shape& shape, Graphics& graphics, const StagingBufferManager& manager, const HeapBuffer<Vertex2>& vertices, const HeapBuffer<u16>& indices, const FColor& color)
{
	rv_result;
	Data& data = graphics.GetData(shape);
	data.vertices.data = vertices;
	data.indices.data = indices;
	data.color = color;
	rv_rif(VertexBuffer::Create(
		data.vertexBuffer,
		data.vertices,
		manager,
		data.vertices.data.data(),
		data.vertices.ByteSize()
	));
	rv_rif(IndexBuffer::Create(
		data.indexBuffer,
		data.indices,
		manager,
		data.indices.data.data(),
		data.indices.ByteSize(),
		VK_INDEX_TYPE_UINT16
	));
	return result;
}

rv::Result rv::Shape::Create(Shape& shape, Graphics& graphics, const StagingBufferManager& manager, HeapBuffer<Vertex2>&& vertices, HeapBuffer<u16>&& indices, const FColor& color)
{
	rv_result;
	Data& data = graphics.GetData(shape);
	data.vertices.data = std::move(vertices);
	data.indices.data = std::move(indices);
	data.color = color;
	rv_rif(VertexBuffer::Create(
		data.vertexBuffer,
		data.vertices,
		manager,
		data.vertices.data.data(),
		data.vertices.ByteSize()
	));
	rv_rif(IndexBuffer::Create(
		data.indexBuffer,
		data.indices,
		manager,
		data.indices.data.data(),
		data.indices.ByteSize(),
		VK_INDEX_TYPE_UINT16
	));
	return result;
}

rv::Result rv::Shape::InitStaticData(Graphics& graphics, DescriptorSetAllocator& allocator)
{
	StaticData& staticData = graphics.GetStaticData<Shape>();
	DescriptorSetBindings bindings;
	bindings.AddBinding(RV_ST_FRAGMENT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	return allocator.GetQueue(staticData.queue, bindings);
}

rv::Result rv::Shape::InitImageData(Shape& shape, Graphics& graphics, Renderer& renderer, DescriptorSetAllocator& allocator, const StagingBufferManager& manager, u32 imageCount)
{
	rv_result;
	Data& data = graphics.GetData(shape);
	StaticData& staticData = graphics.GetStaticData<Shape>();
	for (u32 i = 0; i < imageCount; ++i)
	{
		ImageData& image = renderer.GetImageData(shape, i);
		rv_rif(UniformBuffer::Create(image.buffer, image.color, manager, &data.color, sizeof(FColor)));
		rv_rif(allocator.Allocate(image.set, *staticData.queue));
		image.set.Write(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, image.buffer, sizeof(FColor), 0, 0);
	}
	return result;
}

void rv::Shape::RecordCommand(CommandBuffer& draw, Graphics& graphics, Renderer& renderer, const DrawableRecorder& recorder, u32 image)
{
	const Data& data = graphics.GetDataInterpreted<Shape>(recorder.drawable);
	draw.BindDescriptorSet(renderer.GetImageDataInterpreted<Shape>(recorder.drawable, image).set, recorder.pipeline->layout);
	draw.BindVertexBuffer(data.vertexBuffer);
	draw.BindIndexBuffer(data.indexBuffer);
	draw.DrawIndexed(data.indices.Size());
}

void rv::Shape::DescribePipeline(Graphics& graphics, PipelineLayoutDescriptor& layout, u32 index)
{
	layout.shaders = {
		"triangle.vert",
		"triangle.frag"
	};
	layout.cullMode = VK_CULL_MODE_NONE;
	layout.clockwise = true;
	layout.vertex.Set<Vertex2>();
	layout.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	layout.AddLayout(graphics.GetStaticData<Shape>().queue->layout);
}