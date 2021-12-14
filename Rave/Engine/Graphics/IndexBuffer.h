#pragma once
#include "Engine/Graphics/Buffer.h"

namespace rv
{
	struct IndexBuffer : public Buffer
	{
		IndexBuffer() = default;
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer(IndexBuffer&& rhs) noexcept;
		~IndexBuffer();

		IndexBuffer& operator= (const IndexBuffer&) = delete;
		IndexBuffer& operator= (IndexBuffer&& rhs) noexcept;

		static Result Create(IndexBuffer& buffer, const MemoryAllocator& allocator, u64 size, VkIndexType type, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
		static Result Create(IndexBuffer& buffer, const MemoryAllocator& allocator, const void* data, u64 size, VkIndexType type, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
		static Result Create(IndexBuffer& buffer, const StagingBufferManager& manager, const void* data, u64 size, VkIndexType type, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
		static Result Create(IndexBuffer& buffer, StagingBuffer& staging, const StagingBufferManager& manager, const void* data, u64 size, VkIndexType type, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
		static Result Create(IndexBuffer& buffer, const MemoryAllocator& allocator, u64 size, VkIndexType type);
		static Result Create(IndexBuffer& buffer, const MemoryAllocator& allocator, const void* data, u64 size, VkIndexType type);
		static Result Create(IndexBuffer& buffer, const StagingBufferManager& manager, const void* data, u64 size, VkIndexType type);
		static Result Create(IndexBuffer& buffer, StagingBuffer& staging, const StagingBufferManager& manager, const void* data, u64 size, VkIndexType type);

		VkIndexType type = VK_INDEX_TYPE_UINT32;
	};
}