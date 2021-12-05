#include "Engine/Graphics/Frame.h"
#include "Engine/Utility/Error.h"

rv::Frame::Frame(SwapChain& swap)
	:
	swap(&swap)
{
}

void rv::Frame::SetSwapChain(SwapChain& s)
{
	swap = &s;
}

rv::Result rv::Frame::Start(u32& image, bool& resized)
{
	rv_result;
	rif_assert(swap);

	inFlight.Wait();

	result = swap->NextImage(image, resized, &imageAvailable);
	this->image = image;

	rv_rif(result);

	if (swap->imagesInFlight[image] != VK_NULL_HANDLE)
		vkWaitForFences(swap->device->device, 1, &swap->imagesInFlight[image], VK_TRUE, UINT64_MAX);

	swap->imagesInFlight[image] = inFlight.fence;

	{
		rv_result;
		rif_assert(!first);
		first = true;
	}

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

rv::Result rv::Frame::End(bool& resized)
{
	return swap->Present(image, renderFinished, resized);
}

rv::Result rv::Frame::Create(Frame& frame, const Device& device, SwapChain& swap)
{
	rv_result;

	frame.SetSwapChain(swap);
	rv_rif(Semaphore::Create(frame.imageAvailable, device));
	rv_rif(Semaphore::Create(frame.renderFinished, device));
	return Fence::Create(frame.inFlight, device, true);
}
