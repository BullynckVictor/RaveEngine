#pragma once
#include "Engine/Libraries/VulkanMemoryAllocator/vk_mem_alloc.h"
#include "Engine/Graphics/Device.h"

namespace rv
{
	struct MemoryAllocator
	{
		MemoryAllocator() = default;
		MemoryAllocator(const MemoryAllocator&) = delete;
		MemoryAllocator(MemoryAllocator&& rhs) noexcept;
		~MemoryAllocator();

		MemoryAllocator& operator= (const MemoryAllocator&) = delete;
		MemoryAllocator& operator= (MemoryAllocator&& rhs) noexcept;

		void Release();

		static Result Create(MemoryAllocator& allocator, const Instance& instance, const Device& device);

		VmaAllocator allocator = nullptr;
	};
}