#include "Engine/Graphics/CommandBuffer.h"
#include "Engine/Utility/Error.h"

template<>
void rv::destroy(VkCommandBuffer buffer, VkDevice device, VkInstance)
{
}

rv::CommandBuffer::CommandBuffer(CommandBuffer&& rhs) noexcept
	:
	buffer(move(rhs.buffer)),
	device(move(rhs.device))
{
}

rv::CommandBuffer::~CommandBuffer()
{
	Release();
}

rv::CommandBuffer& rv::CommandBuffer::operator=(CommandBuffer&& rhs) noexcept
{
	buffer = move(rhs.buffer);
	device = move(rhs.device);
	return *this;
}

void rv::CommandBuffer::Release()
{
	if (device)
		release(buffer, *device);
}

rv::Result rv::CommandBuffer::Create(CommandBuffer& buffer, const Device& device, const CommandPool& pool)
{
	buffer.Release();

	buffer.device = &device;
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = pool.pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;
	return rv_try_vkr(vkAllocateCommandBuffers(device.device, &allocInfo, &buffer.buffer));
}

rv::Result rv::CommandBuffer::Create(std::vector<std::reference_wrapper<CommandBuffer>> buffers, const Device& device, const CommandPool& pool)
{
	for (auto& buffer : buffers)
		buffer.get().Release();

	std::vector<VkCommandBuffer> cmd(buffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = pool.pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (u32)buffers.size();

	Result result = rv_try_vkr(vkAllocateCommandBuffers(device.device, &allocInfo, cmd.data()));

	if (result.failed())
		return result;

	for (size_t i = 0; i < cmd.size(); ++i)
	{
		buffers[i].get().buffer = cmd[i];
		buffers[i].get().device = &device;
	}

	return result;
}

rv::Result rv::CommandBuffer::Begin(bool once) const
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = once ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;
	return rv_try_vkr(vkBeginCommandBuffer(buffer, &beginInfo));
}

rv::Result rv::CommandBuffer::End() const
{
	return rv_try_vkr(vkEndCommandBuffer(buffer));
}

void rv::CommandBuffer::StartRenderPass(const RenderPass& pass, const FrameBuffer& frame, const Point& offset, const Extent2D& size, const FColor& color) const
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = pass.pass;
	renderPassInfo.framebuffer = frame.frame;
	renderPassInfo.renderArea.offset = { offset.x, offset.y };
	renderPassInfo.renderArea.extent = { size.width, size.height };
	VkClearValue clearColor{};
	clearColor.color = { { color.r, color.g, color.b, color.a } };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void rv::CommandBuffer::EndRenderPass() const
{
	vkCmdEndRenderPass(buffer);
}

void rv::CommandBuffer::BindPipeline(const Pipeline& pipeline) const
{
	vkCmdBindPipeline(buffer, (VkPipelineBindPoint)pipeline.type, pipeline.pipeline);
}

void rv::CommandBuffer::BindVertexBuffer(const VertexBuffer& vertices) const
{
	u64 offset = 0;
	vkCmdBindVertexBuffers(buffer, 0, 1, &vertices.buffer, &offset);
}

void rv::CommandBuffer::BindIndexBuffer(const IndexBuffer& indices) const
{
	vkCmdBindIndexBuffer(buffer, indices.buffer, 0, indices.type);
}

void rv::CommandBuffer::BindDescriptorSet(const DescriptorSet& set, const PipelineLayout& layout, PipelineType type)
{
	vkCmdBindDescriptorSets(buffer, (VkPipelineBindPoint)type, layout.layout, 0, 1, &set.set, 0, nullptr);
}

void rv::CommandBuffer::Draw(u32 nVertices, u32 nInstances, u32 vertexOffset, u32 instanceOffset) const
{
	vkCmdDraw(buffer, nVertices, nInstances, vertexOffset, instanceOffset);
}

void rv::CommandBuffer::DrawIndexed(u32 nIndices, u32 nInstances, u32 vertexOffset, u32 indexOffset, u32 instanceOffset) const
{
	vkCmdDrawIndexed(buffer, nIndices, nInstances, indexOffset, vertexOffset, instanceOffset);
}

void rv::CommandBuffer::CopyBuffers(const Buffer& source, const Buffer& dest, u64 size, u64 srcOffset, u64 destOffFset)
{
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = srcOffset; // Optional
	copyRegion.dstOffset = destOffFset; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(buffer, source.buffer, dest.buffer, 1, &copyRegion);
}

rv::Result rv::CommandBuffer::Submit(const Fence* fence) const
{
	return Submit(device->graphicsQueue, fence);
}

rv::Result rv::CommandBuffer::Submit(const Queue& queue, const Fence* fence) const
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buffer;
	return rv_try_vkr(vkQueueSubmit(queue.queue, 1, &submitInfo, fence ? fence->fence : VK_NULL_HANDLE));
}

rv::Result rv::CommandBuffer::Submit(const Queue& queue, const std::vector<VkSemaphore>& waitSemaphores, const std::vector<VkSemaphore>& signalSemaphores, const std::vector<VkPipelineStageFlags>& waitStages, const Fence* fence) const
{
	rv_result;

	rif_assert(waitSemaphores.size() == waitStages.size());

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buffer;
	submitInfo.signalSemaphoreCount = (u32)signalSemaphores.size();
	submitInfo.pSignalSemaphores = signalSemaphores.data();
	submitInfo.waitSemaphoreCount = (u32)waitSemaphores.size();
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.pWaitDstStageMask = waitStages.data();

	return rv_try_vkr(vkQueueSubmit(queue.queue, 1, &submitInfo, fence ? fence->fence : VK_NULL_HANDLE));
}

rv::Result rv::CommandBuffer::Submit(const Queue& queue, const Semaphore& waitSemaphore, const Semaphore& signalSemaphore, const VkPipelineStageFlags& waitStage, const Fence* fence) const
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &signalSemaphore.semaphore;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &waitSemaphore.semaphore;
	submitInfo.pWaitDstStageMask = &waitStage;

	return rv_try_vkr(vkQueueSubmit(queue.queue, 1, &submitInfo, fence ? fence->fence : VK_NULL_HANDLE));
}

rv::Result rv::CommandBuffer::Submit(const Queue& queue, const std::vector<std::reference_wrapper<const Semaphore>>& waitSemaphores, const std::vector<std::reference_wrapper<const Semaphore>>& signalSemaphores, const std::vector<VkPipelineStageFlags>& waitStages, const Fence* fence) const
{
	std::vector<VkSemaphore> wSemaphores(waitSemaphores.size());
	std::vector<VkSemaphore> sSemaphores(signalSemaphores.size());

	auto transform = [](const Semaphore& semaphore) -> VkSemaphore {
		return semaphore.semaphore;
	};

	std::transform(waitSemaphores.begin(), waitSemaphores.end(), wSemaphores.begin(), transform);
	std::transform(signalSemaphores.begin(), signalSemaphores.end(), sSemaphores.begin(), transform);

	return Submit(queue, wSemaphores, sSemaphores, waitStages, fence);
}

rv::Result rv::CommandBuffer::Submit(const SubmitInfo& submitInfo, const Queue& queue, const Fence* fence)
{
	rv_result;
	rif_assert(submitInfo.waitSemaphores.size() == submitInfo.waitStages.size());

	VkSubmitInfo vkSubmitInfo{};
	submitInfo.Fill(vkSubmitInfo);

	return rv_try_vkr(vkQueueSubmit(queue.queue, 1, &vkSubmitInfo, fence ? fence->fence : VK_NULL_HANDLE));
}

rv::Result rv::CommandBuffer::Submit(const std::vector<SubmitInfo>& submitInfos, const Queue& queue, const Fence* fence)
{
	rv_result;

	std::vector<VkSubmitInfo> submitInfo(submitInfos.size());

	for (size_t i = 0; i < submitInfos.size(); ++i)
	{
		rif_assert(submitInfos[i].waitSemaphores.size() == submitInfos[i].waitStages.size());
		submitInfos[i].Fill(submitInfo[i]);
	}
	return rv_try_vkr(vkQueueSubmit(queue.queue, (u32)submitInfo.size(), submitInfo.data(), fence ? fence->fence : VK_NULL_HANDLE));
}

rv::Result rv::CommandBuffer::Reset(bool release)
{
	if (buffer)
		return rv_try_vkr(vkResetCommandBuffer(buffer, release ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0));
	return success;
}

void rv::SubmitInfo::AddCommandBuffer(const CommandBuffer& buffer)
{
	buffers.push_back(buffer.buffer);
}

void rv::SubmitInfo::AddWaitSemaphore(const Semaphore& semaphore)
{
	waitSemaphores.push_back(semaphore.semaphore);
}

void rv::SubmitInfo::AddSignalSemaphore(const Semaphore& semaphore)
{
	signalSemaphores.push_back(semaphore.semaphore);
}

void rv::SubmitInfo::AddWaitStage(VkPipelineStageFlags stage)
{
	waitStages.push_back(stage);
}

void rv::SubmitInfo::AddWait(const Semaphore& semaphore, VkPipelineStageFlags stage)
{
	AddWaitSemaphore(semaphore);
	AddWaitStage(stage);
}

void rv::SubmitInfo::Fill(VkSubmitInfo& submitInfo) const
{
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = (u32)buffers.size();
	submitInfo.pCommandBuffers = buffers.data();
	submitInfo.signalSemaphoreCount = (u32)signalSemaphores.size();
	submitInfo.pSignalSemaphores = signalSemaphores.data();
	submitInfo.waitSemaphoreCount = (u32)waitSemaphores.size();
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.pWaitDstStageMask = waitStages.data();
}
