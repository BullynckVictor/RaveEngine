#pragma once
#include "Engine/Graphics/CommandPool.h"
#include "Engine/Graphics/RenderPass.h"
#include "Engine/Graphics/FrameBuffer.h"
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Graphics/Fence.h"
#include "Engine/Graphics/Semaphore.h"
#include "Engine/Utility/Color.h"
#include "Engine/Graphics/Buffer.h"
#include "Engine/Graphics/IndexBuffer.h"
#include "Engine/Graphics/DescriptorSet.h"

namespace rv
{
	struct CommandBuffer;

	struct SubmitInfo
	{
		void AddCommandBuffer(const CommandBuffer& buffer);
		void AddWaitSemaphore(const Semaphore& semaphore);
		void AddSignalSemaphore(const Semaphore& semaphore);
		void AddWaitStage(VkPipelineStageFlags stage);
		void AddWait(const Semaphore& semaphore, VkPipelineStageFlags stage);

		void Fill(VkSubmitInfo& submitInfo) const;

		std::vector<VkCommandBuffer> buffers;
		std::vector<VkSemaphore> waitSemaphores;
		std::vector<VkSemaphore> signalSemaphores;
		std::vector<VkPipelineStageFlags> waitStages;
	};

	struct CommandBuffer
	{
		CommandBuffer() = default;
		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer(CommandBuffer&& rhs) noexcept;
		~CommandBuffer();

		CommandBuffer& operator= (const CommandBuffer&) = delete;
		CommandBuffer& operator= (CommandBuffer&& rhs) noexcept;

		void Release();

		static Result Create(CommandBuffer& buffer, const Device& device, const CommandPool& pool);
		static Result Create(std::vector<std::reference_wrapper<CommandBuffer>> buffers, const Device& device, const CommandPool& pool);

		Result Begin(bool once = false) const;
		Result End() const;
		void StartRenderPass(
			const RenderPass& pass,
			const FrameBuffer& frame,
			const Point& offset,
			const Extent2D& size,
			const FColor& color = FColors::Transparent
		) const;
		void EndRenderPass() const;
		void BindPipeline(const Pipeline& pipeline) const;
		void BindVertexBuffer(const VertexBuffer& vertices) const;
		void BindIndexBuffer(const IndexBuffer& indices) const;
		void BindDescriptorSet(const DescriptorSet& set, const PipelineLayout& layout, PipelineType type = RV_PT_GRAPHICS);
		void Draw(u32 nVertices, u32 nInstances = 1, u32 vertexOffset = 0, u32 instanceOffset = 0) const;
		void DrawIndexed(u32 nIndices, u32 nInstances = 1, u32 vertexOffset = 0, u32 indexOffset = 0, u32 instanceOffset = 0) const;

		void CopyBuffers(const Buffer& source, const Buffer& dest, u64 size, u64 srcOffset = 0, u64 destOffset = 0);

		Result Submit(const Fence* fence = nullptr) const;
		Result Submit(const Queue& queue, const Fence* fence = nullptr) const;
		Result Submit(
			const Queue& queue,
			const std::vector<VkSemaphore>& waitSemaphores,
			const std::vector<VkSemaphore>& signalSemaphores,
			const std::vector<VkPipelineStageFlags>& waitStages,
			const Fence* fence = nullptr
		) const;
		Result Submit(
			const Queue& queue,
			const Semaphore& waitSemaphore,
			const Semaphore& signalSemaphore,
			const VkPipelineStageFlags& waitStage,
			const Fence* fence = nullptr
		) const;
		Result Submit(
			const Queue& queue,
			const std::vector<std::reference_wrapper<const Semaphore>>& waitSemaphores,
			const std::vector<std::reference_wrapper<const Semaphore>>& signalSemaphores,
			const std::vector<VkPipelineStageFlags>& waitStages,
			const Fence* fence = nullptr
		) const;
		static Result Submit(const SubmitInfo& submitInfo, const Queue& queue, const Fence* fence = nullptr);
		static Result Submit(const std::vector<SubmitInfo>& submitInfos, const Queue& queue, const Fence* fence = nullptr);

		Result Reset(bool release = false);

		VkCommandBuffer buffer = VK_NULL_HANDLE;
		const Device* device = nullptr;
	};
}