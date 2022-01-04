#pragma once
#include "Engine/Graphics/Buffer.h"
#include "Engine/Graphics/CommandBuffer.h"
#include "Engine/Graphics/Fence.h"
#include "Engine/Utility/HeapBuffer.h"

namespace rv
{
	struct StagingBufferManager
	{
		StagingBufferManager() = default;
		StagingBufferManager(const StagingBufferManager&) = delete;
		StagingBufferManager(StagingBufferManager&& rhs) noexcept;

		StagingBufferManager& operator= (const StagingBufferManager&) = delete;
		StagingBufferManager& operator= (StagingBufferManager&& rhs) noexcept;

		void Release();

		static Result Create(StagingBufferManager& manager, const Device& device, const MemoryAllocator& allocator, const Queue& transferQueue);

		CommandPool pool;
		const MemoryAllocator* allocator;
		const Device* device;
		Queue transferQueue;
	};

	struct StagingBuffer : public Buffer
	{
		StagingBuffer() = default;
		StagingBuffer(const StagingBuffer&) = delete;
		StagingBuffer(StagingBuffer&& rhs) noexcept;
		~StagingBuffer();

		StagingBuffer& operator= (const StagingBuffer&) = delete;
		StagingBuffer& operator= (StagingBuffer&& rhs) noexcept;

		void Release();

		static Result Create(
			StagingBuffer& staging, 
			const StagingBufferManager& manager,
			const Buffer& destination, 
			const void* data,
			u64 size, 
			bool once,
			u64 srcOffset = 0, 
			u64 dstOffset = 0
		);

		Result Copy() const;
		Result Copy(const Fence& fence) const;

		CommandBuffer copyCommand;
		const StagingBufferManager* manager = nullptr;
	};

	template<typename T>
	struct MappedStagingBuffer : public StagingBuffer
	{
		MappedStagingBuffer() = default;
		MappedStagingBuffer(const MappedStagingBuffer&) = delete;
		MappedStagingBuffer(MappedStagingBuffer&& rhs) noexcept
			:
			StagingBuffer(std::move(rhs)),
			data(std::move(rhs.data))
		{}
		~MappedStagingBuffer()
		{
			Release();
		}

		MappedStagingBuffer& operator= (const MappedStagingBuffer&) = delete;
		MappedStagingBuffer& operator= (MappedStagingBuffer&& rhs) noexcept
		{
			detail::move_buffers<StagingBuffer>(*this, rhs);
			data = std::move(rhs.data);
			return *this;
		}

		void Release()
		{
			StagingBuffer::Release();
			data.clear();
		}

		static constexpr u64 ElementSize()
		{
			return sizeof(T);
		}
		u32 Size() const
		{
			return (u32)data.size();
		}
		u64 ByteSize()
		{
			return (u64)Size() * ElementSize();
		}

		static Result Create(
			MappedStagingBuffer& staging,
			const StagingBufferManager& manager,
			const Buffer& destination,
			const HeapBuffer<T>& data,
			bool once = false,
			u64 srcOffset = 0,
			u64 dstOffset = 0
		)
		{
			staging.data = data;
			return StagingBuffer::Create(staging, manager, destination, staging.data.data(), ByteSize(), once, srcOffset * ElementSize(), dstOffset * ElementSize());
		}
		static Result Create(
			MappedStagingBuffer& staging,
			const StagingBufferManager& manager,
			const Buffer& destination,
			HeapBuffer<T>&& data,
			bool once = false,
			u64 srcOffset = 0,
			u64 dstOffset = 0
		)
		{
			staging.data = std::move(data);
			return StagingBuffer::Create(staging, manager, destination, staging.data.data(), ByteSize(), once, srcOffset * ElementSize(), dstOffset * ElementSize());
		}

		Result Map() const
		{
			return Buffer::Map(data.data(), ByteSize());
		}
		Result Copy() const
		{
			Result result = Map();
			if (result.failed())
				return result;
			return StagingBuffer::Copy();
		}
		Result Copy(const Fence& fence) const
		{
			Result result = Map();
			if (result.failed())
				return result;
			return StagingBuffer::Copy(fence);
		}

		HeapBuffer<T> data;
	};
}