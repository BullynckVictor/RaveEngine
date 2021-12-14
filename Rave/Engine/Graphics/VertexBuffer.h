#pragma once
#include "Engine/Graphics/Buffer.h"
#include "Engine/Graphics/Vertex.h"
#include "Engine/Utility/HeapBuffer.h"

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

		static Result Create(VertexBuffer& buffer, const MemoryAllocator& allocator, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
		static Result Create(VertexBuffer& buffer, const MemoryAllocator& allocator, const void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
		static Result Create(VertexBuffer& buffer, const StagingBufferManager& manager, const void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
		static Result Create(VertexBuffer& buffer, StagingBuffer& staging, const StagingBufferManager& manager, const void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
		static Result Create(VertexBuffer& buffer, const MemoryAllocator& allocator, u64 size);
		static Result Create(VertexBuffer& buffer, const MemoryAllocator& allocator, const void* data, u64 size);
		static Result Create(VertexBuffer& buffer, const StagingBufferManager& manager, const void* data, u64 size);
		static Result Create(VertexBuffer& buffer, StagingBuffer& staging, const StagingBufferManager& manager, const void* data, u64 size);
	};
}
