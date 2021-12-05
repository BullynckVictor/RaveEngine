#include "Engine/Graphics/ImageView.h"
#include "Engine/Utility/Error.h"

template<>
void rv::destroy(VkImageView view, VkDevice device, VkInstance instance)
{
	vkDestroyImageView(device, view, nullptr);
}

rv::ImageView::ImageView(ImageView&& rhs) noexcept
	:
	view(move(rhs.view)),
	device(move(rhs.device))
{
}

rv::ImageView::~ImageView()
{
	Release();
}

rv::ImageView& rv::ImageView::operator=(ImageView&& rhs) noexcept
{
	view = move(rhs.view);
	device = move(rhs.device);
	return *this;
}

void rv::ImageView::Release()
{
	if (device)
		release(view, *device);
}

rv::Result rv::ImageView::Create(ImageView& view, const Device& device, VkImage image, VkFormat format)
{
	view.Release();

	view.device = &device;
	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = format;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;
	return rv_try_vkr(vkCreateImageView(device.device, &createInfo, nullptr, &view.view));
}
