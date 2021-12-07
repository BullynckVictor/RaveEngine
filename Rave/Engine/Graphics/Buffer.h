#pragma once
#include "Engine/Graphics/Allocation.h"

namespace rv
{
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
		static Result Create(Buffer& buffer, const MemoryAllocator& allocator, void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);

		Result Map(void* data, u64 size);

		VkBuffer buffer = VK_NULL_HANDLE;
		Allocation allocation;
	};

	namespace detail
	{
		template<typename B = Buffer>
		void move_buffers(B& a, B& b)
		{
			a = std::move(b);
		}
	}
}