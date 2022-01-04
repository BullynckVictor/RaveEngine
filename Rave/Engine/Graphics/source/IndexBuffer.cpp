#include "Engine/Graphics/IndexBuffer.h"

rv::IndexBuffer::IndexBuffer(IndexBuffer&& rhs) noexcept
	:
	Buffer(std::move(rhs))
{
}

rv::IndexBuffer::~IndexBuffer()
{
	Release();
}

rv::IndexBuffer& rv::IndexBuffer::operator=(IndexBuffer&& rhs) noexcept
{
	detail::move_buffers<Buffer>(*this, rhs);
	return *this;
}

rv::Result rv::IndexBuffer::Create(IndexBuffer& buffer, const MemoryAllocator& allocator, u64 size, VkIndexType type, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	buffer.type = type;
	return Buffer::Create(buffer, allocator, size, bufferUsage | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, memoryUsage);
}

rv::Result rv::IndexBuffer::Create(IndexBuffer& buffer, const MemoryAllocator& allocator, const void* data, u64 size, VkIndexType type, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	buffer.type = type;
	return Buffer::Create(buffer, allocator, data, size, bufferUsage | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, memoryUsage);
}

rv::Result rv::IndexBuffer::Create(IndexBuffer& buffer, const StagingBufferManager& manager, const void* data, u64 size, VkIndexType type, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	buffer.type = type;
	return Buffer::Create(buffer, manager, data, size, bufferUsage | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, memoryUsage);
}

rv::Result rv::IndexBuffer::Create(IndexBuffer& buffer, StagingBuffer& staging, const StagingBufferManager& manager, const void* data, u64 size, VkIndexType type, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	buffer.type = type;
	return Buffer::Create(buffer, staging, manager, data, size, bufferUsage | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, memoryUsage);
}
