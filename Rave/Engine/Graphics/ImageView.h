#pragma once
#include "Engine/Graphics/Device.h"

namespace rv
{
	struct ImageView
	{
		ImageView() = default;
		ImageView(const ImageView&) = default;
		ImageView(ImageView&& rhs) noexcept;
		~ImageView();

		ImageView& operator= (const ImageView&) = default;
		ImageView& operator= (ImageView&& rhs) noexcept;

		void Release();

		static Result Create(ImageView& view, const Device& device, VkImage image, VkFormat format);

		VkImageView view = VK_NULL_HANDLE;
		const Device* device = nullptr;
	};
}