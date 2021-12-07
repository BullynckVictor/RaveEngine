#pragma once
#include "Engine/Graphics/Buffer.h"
#include "Engine/Graphics/Vertex.h"

namespace rv
{
	struct VertexBuffer : public Buffer
	{
		VertexBuffer() = default;
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&& rhs) noexcept;
		~VertexBuffer();

		VertexBuffer& operator= (const VertexBuffer&) = delete;
		VertexBuffer& operator= (VertexBuffer&& rhs) noexcept;

		static Result Create(VertexBuffer& buffer, const MemoryAllocator& allocator, u64 size, VkBufferUsageFlags bufferUsage = 0, VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY);
		static Result Create(VertexBuffer& buffer, const MemoryAllocator& allocator, void* data, u64 size, VkBufferUsageFlags bufferUsage = 0, VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY);
	};

	template<VertexConcept V>
	struct MappedVertexBuffer : public VertexBuffer
	{
		MappedVertexBuffer() = default;
		MappedVertexBuffer(const MappedVertexBuffer&) = delete;
		MappedVertexBuffer(MappedVertexBuffer&& rhs) noexcept
			: 
			VertexBuffer(std::move(rhs)),
			vertices(std::move(rhs.vertices))
		{}
		~MappedVertexBuffer()
		{
			Release();
		}

		MappedVertexBuffer& operator= (const MappedVertexBuffer&) = delete;
		MappedVertexBuffer& operator= (MappedVertexBuffer&& rhs) noexcept
		{
			detail::move_buffers<VertexBuffer>(*this, rhs);
			vertices = std::move(rhs.vertices);
			return *this;
		}

		void Release()
		{
			VertexBuffer::Release();
			vertices.clear();
		}

		static constexpr u64 ElementSize()
		{
			return sizeof(V);
		}
		u32 Size() const
		{
			return (u32)vertices.size();
		}
		u64 ByteSize()
		{
			return (u64)Size() * ElementSize();
		}

		static Result Create(MappedVertexBuffer& buffer, const MemoryAllocator& allocator, u64 size, VkBufferUsageFlags bufferUsage = 0, VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU)
		{
			buffer.vertices.resize(size);
			return VertexBuffer::Create(buffer, allocator, buffer.ByteSize(), bufferUsage, memoryUsage);
		}
		static Result Create(MappedVertexBuffer& buffer, const MemoryAllocator& allocator, const std::vector<V>& vertices, VkBufferUsageFlags bufferUsage = 0, VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU)
		{
			buffer.vertices = vertices;
			return VertexBuffer::Create(buffer, allocator, buffer.vertices.data(), buffer.ByteSize(), bufferUsage, memoryUsage);
		}
		static Result Create(MappedVertexBuffer& buffer, const MemoryAllocator& allocator, std::vector<V>&& vertices, VkBufferUsageFlags bufferUsage = 0, VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU)
		{
			buffer.vertices = std::move(vertices);
			return VertexBuffer::Create(buffer, allocator, buffer.vertices.data(), buffer.ByteSize(), bufferUsage, memoryUsage);
		}

		Result Map()
		{
			return Map(vertices.data(), ByteSize());
		}
	
		std::vector<V> vertices;
	};
}