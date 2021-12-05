#include "Engine/Graphics/RenderPass.h"
#include "Engine/Utility/Error.h"

template<>
void rv::destroy(VkRenderPass pass, VkDevice device, VkInstance)
{
	vkDestroyRenderPass(device, pass, nullptr);
}

rv::RenderPass::RenderPass(RenderPass&& rhs) noexcept
	:
	pass(move(rhs.pass)),
	device(move(rhs.device))
{
}

rv::RenderPass::~RenderPass()
{
	Release();
}

rv::RenderPass& rv::RenderPass::operator=(RenderPass&& rhs) noexcept
{
	pass = move(rhs.pass);
	device = move(rhs.device);
	return *this;
}

rv::Result rv::RenderPass::Create(RenderPass& pass, const Device& device, const RenderPassDescriptor& descriptor)
{
	pass.Release();
	pass.device = &device;

	std::vector<VkSubpassDescription> sub (descriptor.subpasses.size());
	std::transform(descriptor.subpasses.begin(), descriptor.subpasses.end(), sub.begin(), [](const Subpass& subpass) { 
		VkSubpassDescription s{};
		s.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		s.colorAttachmentCount = (u32)subpass.color.size();
		s.pColorAttachments = subpass.color.data();
		s.inputAttachmentCount = (u32)subpass.input.size();
		s.pInputAttachments = subpass.input.data();
		s.pResolveAttachments = subpass.resolve.data();
		s.pDepthStencilAttachment = subpass.useDepth ? &subpass.depthStencil : nullptr;
		s.preserveAttachmentCount = (u32)subpass.preserve.size();
		s.pPreserveAttachments = subpass.preserve.data();
		return s;
	});

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = (u32)descriptor.attachments.size();
	renderPassInfo.pAttachments = descriptor.attachments.data();
	renderPassInfo.subpassCount = (u32)sub.size();
	renderPassInfo.pSubpasses = sub.data();
	renderPassInfo.dependencyCount = (u32)descriptor.dependencies.size();
	renderPassInfo.pDependencies = descriptor.dependencies.data();

	return rv_try_vkr(vkCreateRenderPass(device.device, &renderPassInfo, nullptr, &pass.pass));
}

void rv::RenderPass::Release()
{
	if (device)
		release(pass, *device);
}

rv::u32 rv::RenderPassDescriptor::AddSubpass()
{
	u32 index = (u32)subpasses.size();
	subpasses.emplace_back();
	return index;
}

void rv::RenderPassDescriptor::AddColorAttachment(u32 subpass, VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp)
{
	attachments.push_back({});
	VkAttachmentDescription& colorAttachment = attachments.back();
	colorAttachment.format = format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = loadOp;
	colorAttachment.storeOp = storeOp;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference ref{};
	ref.attachment = (u32)attachments.size() - 1;
	ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	subpasses[subpass].color.push_back(ref);
}

void rv::RenderPassDescriptor::AddDependency(const VkSubpassDependency& dependency)
{
	dependencies.push_back(dependency);
}

void rv::RenderPassDescriptor::AddColorDependency(u32 subpass)
{
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	AddDependency(dependency);
}
