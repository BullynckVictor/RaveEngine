#include "Engine/Graphics/SwapChain.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"
#include "Engine/Core/Logger.h"

template<>
void rv::destroy(VkSwapchainKHR swap, VkDevice device, VkInstance instance)
{
	vkDestroySwapchainKHR(device, swap, nullptr);
}

rv::SwapChain::SwapChain(SwapChain&& rhs) noexcept
	:
	swap(move(rhs.swap)),
	surface(std::move(rhs.surface)),
	presentQueue(std::move(rhs.presentQueue)),
	images(std::move(rhs.images)),
	views(std::move(rhs.views)),
	format(rhs.format),
	presentMode(rhs.presentMode),
	device(move(rhs.device))
{
}

rv::SwapChain& rv::SwapChain::operator= (SwapChain&& rhs) noexcept
{
	swap = move(rhs.swap);
	surface = std::move(rhs.surface);
	presentQueue = std::move(rhs.presentQueue);
	images = std::move(rhs.images);
	views = std::move(rhs.views);
	format = rhs.format;
	presentMode = rhs.presentMode;
	device = move(rhs.device);
	return *this;
}

rv::SwapChain::~SwapChain()
{
	Release();
}

void rv::SwapChain::Release()
{
	presentQueue.Wait();
	if (device)
		release(swap, *device);
	surface.Release();
	presentQueue.Release();
	images.clear();
	views.clear();
	device = nullptr;
}

rv::Result rv::SwapChain::Create(SwapChain& swap, const Device& device, Surface&& surface, const Extent2D& size, const SwapChainPreferences& preferences)
{
	swap.Release();
	rv_result;

	swap.device = &device;

	SwapChainSupportDetails support;
	rv_rif(SwapChainSupportDetails::Create(support, surface, device.physical));
	rif_assert(support.formats.size());

	swap.presentQueue = device.GetQueue(QueueFamilyGetter(RV_QUEUE_GETTER_TYPE_PRESENT, surface.surface));

	swap.format = support.formats.front();
	swap.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	VkExtent2D extent = support.capabilities.currentExtent;

	for (const auto& availableFormat : support.formats)
		if (availableFormat.format == preferences.format.format && availableFormat.colorSpace == preferences.format.colorSpace)
		{
			swap.format = availableFormat;
			break;
		}

	for (const auto& availablePresentMode : support.presentModes)
		if (availablePresentMode == preferences.presentMode) 
		{
			swap.presentMode = availablePresentMode;
			break;
		}

	if (support.capabilities.currentExtent.width == std::numeric_limits<u32>::max())
		extent = { 
			std::clamp(size.width,  support.capabilities.minImageExtent.width,  support.capabilities.maxImageExtent.width), 
			std::clamp(size.height, support.capabilities.minImageExtent.height, support.capabilities.maxImageExtent.height)
		};

	u32 imageCount = std::clamp(
		preferences.imageCount, 
		support.capabilities.minImageCount, 
		support.capabilities.maxImageCount ? support.capabilities.maxImageCount : std::numeric_limits<u32>::max()
	);


	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface.surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = swap.format.format;
	createInfo.imageColorSpace = swap.format.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; 
	createInfo.preTransform = support.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = swap.presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = swap.swap;

	rif_try_vkr_info(vkCreateSwapchainKHR(device.device, &createInfo, nullptr, &swap.swap), "Unable to create SwapChain");
	rv_log("Created SwapChain");

	rif_try_vkr(vkGetSwapchainImagesKHR(device.device, swap.swap, &imageCount, nullptr));
	swap.images.resize(imageCount);
	rif_try_vkr(vkGetSwapchainImagesKHR(device.device, swap.swap, &imageCount, swap.images.data()));

	swap.views.resize(swap.images.size());
	for (size_t i = 0; i < swap.images.size(); ++i)
		rv_rif(ImageView::Create(swap.views[i], device, swap.images[i], swap.format.format));

	swap.surface = std::move(surface);
	swap.imagesInFlight.resize(swap.images.size(), VK_NULL_HANDLE);

	return result;
}

rv::Result rv::SwapChain::Create(SwapChain& swap, const Instance& instance, const Device& device, const Window& window, const SwapChainPreferences& preferences)
{
	Surface surface;
	Surface::Create(surface, instance, window);
	return Create(swap, device, std::move(surface), window.Size(), preferences);
}

rv::Result rv::SwapChain::NextImage(u32& image, bool& resized, const Semaphore* semaphore, const Fence* fence, u64 wait) const
{
	resized = false;
	VkResult vkr = vkAcquireNextImageKHR(device->device, swap, wait, semaphore ? semaphore->semaphore : nullptr, fence ? fence->fence : nullptr, &image);
	if (vkr == VK_ERROR_OUT_OF_DATE_KHR || vkr == VK_SUBOPTIMAL_KHR)
	{
		resized = true;
		return succeeded_vkr;
	}
	return rv_try_vkr(vkr);
}

rv::Result rv::SwapChain::Present(u32 image, const Semaphore& wait, bool& resized)
{
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &wait.semaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swap;
	presentInfo.pImageIndices = &image;
	VkResult vkr = vkQueuePresentKHR(presentQueue.queue, &presentInfo);

	if (vkr == VK_ERROR_OUT_OF_DATE_KHR || vkr == VK_SUBOPTIMAL_KHR)
	{
		resized = true;
		return succeeded_vkr;
	}
	resized = false;
	return rv_try_vkr(vkr);
}

rv::Result rv::SwapChainSupportDetails::Create(SwapChainSupportDetails& details, const Surface& surface, const PhysicalDevice& device)
{
	rv_result;

	rif_try_vkr(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.device, surface.surface, &details.capabilities));

	u32 formatCount;
	rif_try_vkr(vkGetPhysicalDeviceSurfaceFormatsKHR(device.device, surface.surface, &formatCount, nullptr));

	if (formatCount) 
	{
		details.formats.resize(formatCount);
		rif_try_vkr(vkGetPhysicalDeviceSurfaceFormatsKHR(device.device, surface.surface, &formatCount, details.formats.data()));
	}

	u32 presentModeCount;
	rif_try_vkr(vkGetPhysicalDeviceSurfacePresentModesKHR(device.device, surface.surface, &presentModeCount, nullptr));

	if (presentModeCount != 0) 
	{
		details.presentModes.resize(presentModeCount);
		rif_try_vkr(vkGetPhysicalDeviceSurfacePresentModesKHR(device.device, surface.surface, &presentModeCount, details.presentModes.data()));
	}

	return result;
}

rv::SwapChainPreferences::SwapChainPreferences(bool vsync, u32 imageCount)
	:
	format({ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }),
	presentMode(vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR),
	imageCount(imageCount)
{
}
