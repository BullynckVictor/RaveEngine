#include "Engine/Graphics/MemoryAllocator.h"
#include "Engine/Utility/Error.h"

rv::MemoryAllocator::MemoryAllocator(MemoryAllocator&& rhs) noexcept
	:
	allocator(move(rhs.allocator))
{
}

rv::MemoryAllocator::~MemoryAllocator()
{
	Release();
}

rv::MemoryAllocator& rv::MemoryAllocator::operator=(MemoryAllocator&& rhs) noexcept
{
	allocator = move(rhs.allocator);
	return *this;
}

void rv::MemoryAllocator::Release()
{
	if (allocator)
		vmaDestroyAllocator(allocator);
}

rv::Result rv::MemoryAllocator::Create(MemoryAllocator& allocator, const Instance& instance, const Device& device)
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.vulkanApiVersion = instance.app.info.apiVersion;
	allocatorInfo.physicalDevice = device.physical.device;
	allocatorInfo.device = device.device;
	allocatorInfo.instance = instance.instance;

	return rv_try_vkr(vmaCreateAllocator(&allocatorInfo, &allocator.allocator));
}
