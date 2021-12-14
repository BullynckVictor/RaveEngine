#include "Engine/Graphics/Buffer.h"
#include "Engine/Graphics/StagingBuffer.h"
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

rv::Result rv::Buffer::Create(Buffer& buffer, const MemoryAllocator& allocator, const void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	rv_result;
	rv_rif(Create(buffer, allocator, size, bufferUsage, memoryUsage));
	if (data)
		return buffer.Map(data, size);
	return success;
}

rv::Result rv::Buffer::Create(Buffer& buffer, const StagingBufferManager& manager, const void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	rv_result;
	rv_rif(Create(buffer, *manager.allocator, size, bufferUsage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, memoryUsage));
	StagingBuffer staging;
	rv_rif(StagingBuffer::Create(staging, manager, buffer, data, size, true));
	return staging.Copy();
}

rv::Result rv::Buffer::Create(Buffer& buffer, StagingBuffer& staging, const StagingBufferManager& manager, const void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	rv_result;
	rv_rif(Create(buffer, *manager.allocator, size, bufferUsage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, memoryUsage));
	rv_rif(StagingBuffer::Create(staging, manager, buffer, data, size, false));
	return staging.Copy();
}

rv::Result rv::Buffer::Map(const void* data, u64 size) const
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