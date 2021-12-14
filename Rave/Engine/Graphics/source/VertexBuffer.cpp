#include "Engine/Graphics/VertexBuffer.h"

rv::VertexBuffer::VertexBuffer(VertexBuffer&& rhs) noexcept
	:
	Buffer(std::move(rhs))
{
}

rv::VertexBuffer::~VertexBuffer()
{
	Release();
}

rv::VertexBuffer& rv::VertexBuffer::operator=(VertexBuffer&& rhs) noexcept
{
	detail::move_buffers<Buffer>(*this, rhs);
	return *this;
}

rv::Result rv::VertexBuffer::Create(VertexBuffer& buffer, const MemoryAllocator& allocator, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	return Buffer::Create(buffer, allocator, size, bufferUsage | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryUsage);
}

rv::Result rv::VertexBuffer::Create(VertexBuffer& buffer, const MemoryAllocator& allocator, const void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	return Buffer::Create(buffer, allocator, data, size, bufferUsage | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryUsage);
}

rv::Result rv::VertexBuffer::Create(VertexBuffer& buffer, const StagingBufferManager& manager, const void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	return Buffer::Create(buffer, manager, data, size, bufferUsage | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryUsage);
}

rv::Result rv::VertexBuffer::Create(VertexBuffer& buffer, StagingBuffer& staging, const StagingBufferManager& manager, const void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	return Buffer::Create(buffer, staging, manager, data, size, bufferUsage | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryUsage);
}

rv::Result rv::VertexBuffer::Create(VertexBuffer& buffer, const MemoryAllocator& allocator, u64 size)
{
	return Create(buffer, allocator, size, 0, VMA_MEMORY_USAGE_GPU_ONLY);
}

rv::Result rv::VertexBuffer::Create(VertexBuffer& buffer, const MemoryAllocator& allocator, const void* data, u64 size)
{
	return Create(buffer, allocator, data, size, 0, VMA_MEMORY_USAGE_GPU_ONLY);
}

rv::Result rv::VertexBuffer::Create(VertexBuffer& buffer, const StagingBufferManager& manager, const void* data, u64 size)
{
	return Create(buffer, manager, data, size, 0, VMA_MEMORY_USAGE_GPU_ONLY);
}

rv::Result rv::VertexBuffer::Create(VertexBuffer& buffer, StagingBuffer& staging, const StagingBufferManager& manager, const void* data, u64 size)
{
	return Create(buffer, staging, manager, data, size, 0, VMA_MEMORY_USAGE_GPU_ONLY);
}
