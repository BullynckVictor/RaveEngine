#pragma once
#include "Engine/Graphics/Device.h"
#include "Engine/Graphics/ImageView.h"
#include "Engine/Core/Window.h"
#include "Engine/Graphics/Semaphore.h"
#include "Engine/Graphics/Fence.h"

namespace rv
{
	struct SwapChainSupportDetails 
	{
		SwapChainSupportDetails() = default;

		static Result Create(SwapChainSupportDetails& details, const Surface& surface, const PhysicalDevice& device);

		VkSurfaceCapabilitiesKHR capabilities = {};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct SwapChainPreferences
	{
		SwapChainPreferences(bool vsync = false, u32 imageCount = 2);

		VkSurfaceFormatKHR format;
		VkPresentModeKHR presentMode;
		u32 imageCount;
	};

	struct SwapChain
	{
		SwapChain() = default;
		SwapChain(const SwapChain&) = delete;
		SwapChain(SwapChain&& rhs) noexcept;
		~SwapChain();

		SwapChain& operator= (const SwapChain&) = delete;
		SwapChain& operator= (SwapChain&& rhs) noexcept;

		void Release();

		static Result Create(SwapChain& swap, const Device& device, Surface&& surface, const Extent2D& size, const SwapChainPreferences& preferences = {});
		static Result Create(SwapChain& swap, const Instance& instance, const Device& device, const Window& window, const SwapChainPreferences& preferences = {});

		Result NextImage(u32& image, bool& resized, const Semaphore* semaphore = nullptr, const Fence* fence = nullptr, u64 wait = std::numeric_limits<u64>::max()) const;
		Result Present(u32 image, const Semaphore& wait, bool& resized);

		VkSwapchainKHR swap = VK_NULL_HANDLE;
		Surface surface;
		Queue presentQueue;
		std::vector<VkImage> images;
		std::vector<ImageView> views;
		VkSurfaceFormatKHR format = {};
		VkPresentModeKHR presentMode = {};
		std::vector<VkFence> imagesInFlight;

		const Device* device = nullptr;
	};
}