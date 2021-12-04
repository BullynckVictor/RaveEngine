#pragma once
#include "Engine/Graphics/Device.h"
#include "Engine/Graphics/RenderPass.h"
#include "Engine/Graphics/ImageView.h"

namespace rv
{
	struct FrameBuffer
	{
		FrameBuffer() = default;
		FrameBuffer(const FrameBuffer&) = default;
		FrameBuffer(FrameBuffer && rhs) noexcept;
		~FrameBuffer();

		FrameBuffer& operator= (const FrameBuffer&) = default;
		FrameBuffer& operator= (FrameBuffer && rhs) noexcept;

		void Release();

		static Result Create(FrameBuffer& frame, const Device& device, const RenderPass& pass, const Extent2D& size, std::vector<VkImageView> views);
		static Result Create(FrameBuffer& frame, const Device& device, const RenderPass& pass, const Extent2D& size, std::vector<std::reference_wrapper<const ImageView>> views);
		static Result Create(FrameBuffer& frame, const Device& device, const RenderPass& pass, const Extent2D& size, const ImageView& view);

		VkFramebuffer frame = VK_NULL_HANDLE;
		const Device* device = nullptr;
	};
}