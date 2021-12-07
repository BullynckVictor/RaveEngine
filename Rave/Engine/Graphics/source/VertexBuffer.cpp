#include "..\VertexBuffer.h"

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
	detail::move_buffers(*this, rhs);
	return *this;
}

rv::Result rv::VertexBuffer::Create(VertexBuffer& buffer, const MemoryAllocator& allocator, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	return Buffer::Create(buffer, allocator, size, bufferUsage | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryUsage);
}

rv::Result rv::VertexBuffer::Create(VertexBuffer& buffer, const MemoryAllocator& allocator, void* data, u64 size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
	return Buffer::Create(buffer, allocator, data, size, bufferUsage | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryUsage);
}