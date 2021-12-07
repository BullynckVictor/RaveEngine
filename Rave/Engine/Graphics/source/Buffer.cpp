#include "Engine/Graphics/Buffer.h"
#include "Engine/Utility/Error.h"

rv::Buffer::Buffer(Buffer&& rhs) noexcept
	:
	buffer(move(rhs.buffer)),
	allocation(std::move(rhs.allocation))
{
}

rv::Buffer::~Buffer()
{
	Release();
}

rv::Buffer& rv::Buffer::operator=(Buffer&& rhs) noexcept
{
	buffer = move(rhs.buffer);
	allocation = std::move(rhs.allocation);
	return *this;
}

void rv::Buffer::Release()
{
	if (allocation.allocator && allocation.allocator->allocator)
	{
		if (buffer)
		{
			vmaDestroyBuffer(allocation.Allocator(), buffer, allocation.allocation);
			allocation.allocation = nullptr;
			buffer = nullptr;
		}
		else
		{
			allocation.Release();
		}
	}
}

rv::Result rv::Buffer::Create(Buffer& buffer, const MemoryAllocator& allocator, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	buffer.Release();
	buffer.allocation = allocator;

	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = size;
	createInfo.usage = bufferUsage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocation{};
	allocation.usage = memoryUsage;

	return rv_try_vkr(vmaCreateBuffer(allocator.allocator, &createInfo, &allocation, &buffer.buffer, &buffer.allocation.allocation, nullptr));
}

rv::Result rv::Buffer::Create(Buffer& buffer, const MemoryAllocator& allocator, void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	rv_result;
	rv_rif(Create(buffer, allocator, size, bufferUsage, memoryUsage));
	return buffer.Map(data, size);
}

rv::Result rv::Buffer::Map(void* data, u64 size)
{
	rv_result;
	rif_assert(data);
	rif_assert(size);
	void* map = nullptr;
	rif_try_vkr(vmaMapMemory(allocation.Allocator(), allocation.allocation, &map));
	memcpy(map, data, size);
	vmaUnmapMemory(allocation.Allocator(), allocation.allocation);
	return result;
}