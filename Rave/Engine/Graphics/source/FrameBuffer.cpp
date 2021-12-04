#include "Engine/Graphics/FrameBuffer.h"
#include "Engine/Utility/Error.h"

template<>
void rv::destroy(VkFramebuffer frame, VkDevice device, VkInstance)
{
	vkDestroyFramebuffer(device, frame, nullptr);
}

rv::FrameBuffer::FrameBuffer(FrameBuffer&& rhs) noexcept
	:
	frame(move(rhs.frame)),
	device(move(rhs.device))
{
}

rv::FrameBuffer::~FrameBuffer()
{
	Release();
}

rv::FrameBuffer& rv::FrameBuffer::operator=(FrameBuffer&& rhs) noexcept
{
	frame = move(rhs.frame);
	device = move(rhs.device);
	return *this;
}

void rv::FrameBuffer::Release()
{
	if (device)
		release(frame, *device);
}

void FillInfo(VkFramebufferCreateInfo& createInfo, const rv::RenderPass& pass, const rv::Extent2D& size)
{
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.renderPass = pass.pass;
	createInfo.width = size.width;
	createInfo.height = size.height;
	createInfo.layers = 1;
}

rv::Result rv::FrameBuffer::Create(FrameBuffer& frame, const Device& device, const RenderPass& pass, const Extent2D& size, std::vector<VkImageView> views)
{
	frame.device = &device;
	VkFramebufferCreateInfo createInfo{};
	FillInfo(createInfo, pass, size);
	createInfo.attachmentCount = (u32)views.size();
	createInfo.pAttachments = views.data();
	return rv_try_vkr(vkCreateFramebuffer(device.device, &createInfo, nullptr, &frame.frame));
}

rv::Result rv::FrameBuffer::Create(FrameBuffer& frame, const Device& device, const RenderPass& pass, const Extent2D& size, std::vector<std::reference_wrapper<const ImageView>> views)
{
	std::vector<VkImageView> iv(views.size());
	std::transform(views.begin(), views.end(), iv.begin(), [](const ImageView& view) { return view.view; });
	return Create(frame, device, pass, size, iv);
}

rv::Result rv::FrameBuffer::Create(FrameBuffer& frame, const Device& device, const RenderPass& pass, const Extent2D& size, const ImageView& view)
{
	frame.device = &device;
	VkFramebufferCreateInfo createInfo{};
	FillInfo(createInfo, pass, size);
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &view.view;
	return rv_try_vkr(vkCreateFramebuffer(device.device, &createInfo, nullptr, &frame.frame));
}
