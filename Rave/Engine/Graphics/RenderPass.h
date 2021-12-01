#pragma once
#include "Engine/Graphics/Device.h"

namespace rv
{
	struct Subpass
	{
		Subpass() = default;

		std::vector<VkAttachmentReference> input;
		std::vector<VkAttachmentReference> color;
		std::vector<VkAttachmentReference> resolve;
		std::vector<u32> preserve;
		VkAttachmentReference depthStencil {};
		bool useDepth = false;
	};

	struct RenderPassDescriptor
	{
		RenderPassDescriptor() = default;

		u32 AddSubpass();
		void AddColorAttachment(u32 subpass, VkFormat format, VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE);

		std::vector<VkAttachmentDescription> attachments;
		std::vector<Subpass> subpasses;
	};

	struct RenderPass
	{
		RenderPass() = default;
		RenderPass(RenderPass&) = delete;
		RenderPass(RenderPass&& rhs) noexcept;
		~RenderPass();

		RenderPass& operator= (const RenderPass&) = delete;
		RenderPass& operator= (RenderPass&& rhs) noexcept;

		static Result Create(RenderPass& pass, const Device& device, const RenderPassDescriptor& descriptor);

		void Release();

		VkRenderPass pass = VK_NULL_HANDLE;
		const Device* device = nullptr;
	};
}