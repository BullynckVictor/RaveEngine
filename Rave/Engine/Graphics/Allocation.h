#pragma once
#include "Engine/Graphics/MemoryAllocator.h"

namespace rv
{
	struct Allocation
	{
		Allocation() = default;
		Allocation(const MemoryAllocator& allocator);
		Allocation(const Allocation&) = delete;
		Allocation(Allocation&& rhs) noexcept;
		~Allocation();

		const VmaAllocation operator->() const;
			  VmaAllocation operator->();

		const VmaAllocator Allocator() const;
			  VmaAllocator Allocator();

		Allocation& operator= (const Allocation&) = delete;
		Allocation& operator= (Allocation&& rhs) noexcept;

		void Release();

		VmaAllocation allocation = nullptr;
		const MemoryAllocator* allocator = nullptr;
	};
}