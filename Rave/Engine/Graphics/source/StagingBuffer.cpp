#include "Engine/Graphics/StagingBuffer.h"
#include "Engine/Utility/Error.h"

rv::StagingBuffer::StagingBuffer(StagingBuffer&& rhs) noexcept
	:
	Buffer(std::move(rhs)),
	copyCommand(std::move(rhs.copyCommand)),
	manager(move(rhs.manager))
{
}

rv::StagingBuffer::~StagingBuffer()
{
	Release();
}

rv::StagingBuffer& rv::StagingBuffer::operator=(StagingBuffer&& rhs) noexcept
{
	detail::move_buffers<Buffer>(*this, rhs);
	copyCommand = std::move(rhs.copyCommand);
	manager = move(rhs.manager);
	return *this;
}

void rv::StagingBuffer::Release()
{
	Buffer::Release();
	copyCommand.Release();
	manager = nullptr;
}

rv::Result rv::StagingBuffer::Create(StagingBuffer& staging, const StagingBufferManager& manager, const Buffer& destination, const void* data, u64 size, bool once, u64 srcOffset, u64 dstOffset)
{
	staging.Release();
	rv_result;

	staging.manager = &manager;

	rv_rif(Buffer::Create(staging, *manager.allocator, data, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY));
	rv_rif(CommandBuffer::Create(staging.copyCommand, *manager.device, manager.pool));
	rv_rif(staging.copyCommand.Begin());
	staging.copyCommand.CopyBuffers(staging, destination, size, srcOffset, dstOffset);
	rv_rif(staging.copyCommand.End());
	return result;
}

rv::Result rv::StagingBuffer::Copy() const
{
	rv_result;
	Fence fence;
	rv_rif(Fence::Create(fence, *copyCommand.device));
	rv_rif(copyCommand.Submit(manager->transferQueue, &fence));
	return fence.Wait();
}

rv::Result rv::StagingBuffer::Copy(const Fence& fence) const
{
	return copyCommand.Submit(manager->transferQueue, &fence);
}

rv::StagingBufferManager::StagingBufferManager(StagingBufferManager&& rhs) noexcept
	:
	pool(std::move(rhs.pool)),
	allocator(move(rhs.allocator)),
	device(move(rhs.device)),
	transferQueue(rhs.transferQueue)
{
}

rv::StagingBufferManager& rv::StagingBufferManager::operator=(StagingBufferManager&& rhs) noexcept
{
	pool = std::move(rhs.pool);
	allocator = move(rhs.allocator);
	device = move(rhs.device);
	transferQueue = rhs.transferQueue;
	return *this;
}

void rv::StagingBufferManager::Release()
{
	pool.Release();
	allocator = nullptr;
	device = nullptr;
	transferQueue.Release();
}

rv::Result rv::StagingBufferManager::Create(StagingBufferManager& manager, const Device& device, const MemoryAllocator& allocator, const Queue& transferQueue)
{
	manager.device = &device;
	manager.allocator = &allocator;
	manager.transferQueue = transferQueue;
	return CommandPool::Create(manager.pool, device, transferQueue.family);
}
