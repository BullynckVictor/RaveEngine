#include "Engine/Graphics/Frame.h"
#include "Engine/Utility/Error.h"

rv::Frame::Frame(SwapChain& swap)
	:
	swap(&swap)
{
}

rv::ResultValue<rv::u32> rv::Frame::NextImage(const Semaphore* semaphore, const Fence* fence)
{
	rv_result;
	rif_assert(swap);
	return swap->NextImage(semaphore, fence, timeout);
}

void rv::Frame::SetSwapChain(SwapChain& s)
{
	swap = &s;
}

rv::ResultValue<rv::u32> rv::Frame::Start()
{
	inFlight.Wait();

	auto result = NextImage(&imageAvailable);
	image = result.value;

	if (swap->imagesInFlight[image] != VK_NULL_HANDLE) {
		vkWaitForFences(swap->device->device, 1, &swap->imagesInFlight[image], VK_TRUE, UINT64_MAX);
	}
	swap->imagesInFlight[image] = inFlight.fence;

	auto a = rv_assert(!first);
	if (a.failed())
		return a;
	first = true;

	return result;
}

rv::Result rv::Frame::Render(const CommandBuffer& drawCommand)
{
	Result result = drawCommand.Submit(drawCommand.device->graphicsQueue, first ? imageAvailable : renderFinished, renderFinished, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	first = false;
	return result;
}

rv::Result rv::Frame::RenderLast(const CommandBuffer& drawCommand)
{
	inFlight.Reset();
	Result result = drawCommand.Submit(drawCommand.device->graphicsQueue, first ? imageAvailable : renderFinished, renderFinished, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, &inFlight);
	first = false;
	return result;
}

rv::Result rv::Frame::End()
{
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinished.semaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swap->swap;
	presentInfo.pImageIndices = &image;
	return rv_try_vkr(vkQueuePresentKHR(swap->presentQueue.queue, &presentInfo));
}

rv::Result rv::Frame::Create(Frame& frame, const Device& device, SwapChain& swap)
{
	rv_result;

	frame.SetSwapChain(swap);
	rv_rif(Semaphore::Create(frame.imageAvailable, device));
	rv_rif(Semaphore::Create(frame.renderFinished, device));
	return Fence::Create(frame.inFlight, device, true);
}
