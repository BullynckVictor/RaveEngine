#include "Engine/Graphics/Allocation.h"
#include "Engine/Utility/Error.h"

rv::Allocation::Allocation(const MemoryAllocator& allocator)
	:
	allocator(&allocator)
{
}

rv::Allocation::Allocation(Allocation&& rhs) noexcept
	:
	allocation(move(rhs.allocation)),
	allocator(move(rhs.allocator))
{
}

rv::Allocation::~Allocation()
{
	Release();
}

const VmaAllocation rv::Allocation::operator->() const
{
	return allocation;
}

VmaAllocation rv::Allocation::operator->()
{
	return allocation;
}

const VmaAllocator rv::Allocation::Allocator() const
{
	return allocator->allocator;
}

VmaAllocator rv::Allocation::Allocator()
{
	return allocator->allocator;
}

rv::Allocation& rv::Allocation::operator=(Allocation&& rhs) noexcept
{
	allocation = move(rhs.allocation);
	allocator = move(rhs.allocator);
	return *this;
}

void rv::Allocation::Release()
{
	if (allocator && allocation)
	{
		vmaFreeMemory(allocator->allocator, allocation);
		allocation = nullptr;
	}
}