#pragma once
#include "Engine/Graphics/Allocation.h"
#include "Engine/Graphics/CommandPool.h"

namespace rv
{
	struct StagingBufferManager;
	struct StagingBuffer;

	struct Buffer
	{
		Buffer() = default;
		Buffer(const Buffer&) = delete;
		Buffer(Buffer&& rhs) noexcept;
		~Buffer();

		Buffer& operator= (const Buffer&) = delete;
		Buffer& operator= (Buffer&& rhs) noexcept;

		void Release();

		static Result Create(Buffer& buffer, const MemoryAllocator& allocator, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
		static Result Create(Buffer& buffer, const MemoryAllocator& allocator, const void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
		static Result Create(Buffer& buffer, const StagingBufferManager& manager, const void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
		static Result Create(Buffer& buffer, StagingBuffer& staging, const StagingBufferManager& manager, const void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);


		Result Map(const void* data, u64 size) const;

		VkBuffer buffer = VK_NULL_HANDLE;
		Allocation allocation;
	};

	namespace detail
	{
		template<typename B = Buffer>
		B& move_buffers(B& a, B& b)
		{
			a = std::move(b);
			return a;
		}
	}

	template<VkBufferUsageFlagBits T, VmaMemoryUsage U = VMA_MEMORY_USAGE_GPU_ONLY>
	struct TypedBuffer : public Buffer
	{
		TypedBuffer() = default;
		TypedBuffer(const TypedBuffer&) = delete;
		TypedBuffer(TypedBuffer&& rhs) noexcept
			:
			Buffer(std::move(rhs))
		{
		}
		~TypedBuffer()
		{
			Release();
		}

		TypedBuffer& operator= (const TypedBuffer&) = delete;
		TypedBuffer& operator= (TypedBuffer&& rhs) noexcept
		{
			return detail::move_buffers<Buffer>(*this, rhs);
		}

		static Result Create(TypedBuffer& buffer, const MemoryAllocator& allocator, u64 size, VkBufferUsageFlags bufferUsage = 0, VmaMemoryUsage memoryUsage = U)
		{
			return Buffer::Create(buffer, allocator, size, bufferUsage | T, memoryUsage);
		}
		static Result Create(TypedBuffer& buffer, const MemoryAllocator& allocator, const void* data, u64 size, VkBufferUsageFlags bufferUsage = 0, VmaMemoryUsage memoryUsage = U)
		{
			return Buffer::Create(buffer, allocator, data, size, bufferUsage | T, memoryUsage);
		}
		static Result Create(TypedBuffer& buffer, const StagingBufferManager& manager, const void* data, u64 size, VkBufferUsageFlags bufferUsage = 0, VmaMemoryUsage memoryUsage = U)
		{
			return Buffer::Create(buffer, manager, data, size, bufferUsage | T, memoryUsage);
		}
		static Result Create(TypedBuffer& buffer, StagingBuffer& staging, const StagingBufferManager& manager, const void* data, u64 size, VkBufferUsageFlags bufferUsage = 0, VmaMemoryUsage memoryUsage = U)
		{
			return Buffer::Create(buffer, staging, manager, data, size, bufferUsage | T, memoryUsage);
		}
	};

	typedef TypedBuffer<VK_BUFFER_USAGE_VERTEX_BUFFER_BIT> VertexBuffer;
	typedef TypedBuffer<VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU> UniformBuffer;
}