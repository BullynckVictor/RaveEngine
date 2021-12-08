#include "Engine/Graphics/Pipeline.h"
#include "Engine/Utility/Error.h"

template<>
void rv::destroy(VkPipelineLayout layout, VkDevice device, VkInstance)
{
	vkDestroyPipelineLayout(device, layout, nullptr);
}

template<>
void rv::destroy(VkPipeline pipeline, VkDevice device, VkInstance)
{
	vkDestroyPipeline(device, pipeline, nullptr);
}

rv::PipelineLayout::PipelineLayout()
{
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.depthBiasEnable = VK_FALSE;

	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	SetSize(0);
	SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	SetLineWidth(1.0f);
	SetCull(true);
}

rv::PipelineLayout::PipelineLayout(PipelineLayout&& rhs) noexcept
	:
	layout(move(rhs.layout)),
	vertexInput(std::move(rhs.vertexInput)),
	inputAssembly(std::move(rhs.inputAssembly)),
	viewport(std::move(rhs.viewport)),
	scissor(std::move(rhs.scissor)),
	viewportState(std::move(rhs.viewportState)),
	rasterizer(std::move(rhs.rasterizer)),
	multisampling(std::move(rhs.multisampling)),
	colorBlendAttachment(std::move(rhs.colorBlendAttachment)),
	colorBlending(std::move(rhs.colorBlending)),
	device(move(rhs.device))
{
}

rv::PipelineLayout::~PipelineLayout()
{
	Release();
}

rv::PipelineLayout& rv::PipelineLayout::operator=(PipelineLayout&& rhs) noexcept
{
	layout = move(rhs.layout);
	vertexInput = std::move(rhs.vertexInput);
	inputAssembly = std::move(rhs.inputAssembly);
	viewport = std::move(rhs.viewport);
	scissor = std::move(rhs.scissor);
	viewportState = std::move(rhs.viewportState);
	rasterizer = std::move(rhs.rasterizer);
	multisampling = std::move(rhs.multisampling);
	colorBlendAttachment = std::move(rhs.colorBlendAttachment);
	colorBlending = std::move(rhs.colorBlending);
	device = move(rhs.device);
	return *this;
}

rv::Result rv::PipelineLayout::Create(PipelineLayout& layout, const Device& device)
{
	layout.Release();
	layout.device = &device;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	return rv_try_vkr(vkCreatePipelineLayout(device.device, &pipelineLayoutInfo, nullptr, &layout.layout));
}

void rv::PipelineLayout::Release()
{
	if (device)
		release(layout, *device);
}

void rv::PipelineLayout::SetToDescriptor(const PipelineLayoutDescriptor& descriptor)
{
	SetTopology(descriptor.topology);
	SetLineWidth(descriptor.lineWidth);
	SetCullMode(descriptor.cullMode);
	SetWinding(descriptor.clockwise);
	SetBlending(descriptor.blending);
	if (descriptor.vertex.attributes)
		SetVertexType(descriptor.vertex);
}

void rv::PipelineLayout::SetTopology(VkPrimitiveTopology topology)
{
	inputAssembly.topology = topology;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void rv::PipelineLayout::SetSize(const Extent2D& size)
{
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)size.width;
	viewport.height = (float)size.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = { 0, 0 };
	scissor.extent = { size.width, size.height };
}

void rv::PipelineLayout::SetLineWidth(float width)
{
	rasterizer.lineWidth = width;
}

void rv::PipelineLayout::SetCullMode(VkCullModeFlags cull)
{
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
}

void rv::PipelineLayout::SetCull(bool cull)
{
	SetCullMode(cull ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE);
}

void rv::PipelineLayout::SetWinding(bool clockwise)
{
	rasterizer.frontFace = clockwise ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE;
}

void rv::PipelineLayout::SetBlending(bool blend)
{
	colorBlendAttachment.blendEnable = blend;
}

void rv::PipelineLayout::AddShader(const Shader& shader)
{
	VkPipelineShaderStageCreateInfo shaderStageInfo{};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = (VkShaderStageFlagBits)Flags<ShaderType, u32>(shader.type).data();
	shaderStageInfo.module = shader.shader;
	shaderStageInfo.pName = "main";
	shaderStages.push_back(shaderStageInfo);
}

void rv::PipelineLayout::AddRenderPass(const RenderPass& pass, u32 subpass)
{
	this->pass = pass.pass;
	this->subpass = subpass;
}

void rv::PipelineLayout::SetVertexType(const VertexDescriptor& vertex)
{
	vertexInput.pVertexAttributeDescriptions = vertex.attributes;
	vertexInput.vertexAttributeDescriptionCount = (u32)vertex.nAttributes;
	vertexInput.pVertexBindingDescriptions = vertex.binding;
	vertexInput.vertexBindingDescriptionCount = 1;
}

rv::Pipeline::Pipeline(Pipeline&& rhs) noexcept
	:
	pipeline(move(rhs.pipeline)),
	device(move(rhs.device))
{
}

rv::Pipeline::~Pipeline()
{
	Release();
}

rv::Pipeline& rv::Pipeline::operator=(Pipeline&& rhs) noexcept
{
	pipeline = move(rhs.pipeline);
	device = move(rhs.device);
	return *this;
}

rv::Result rv::Pipeline::Create(Pipeline& pipeline, const Device& device, const PipelineLayout& layout)
{
	pipeline.Release();
	pipeline.device = &device;
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	FillCreateInfo(pipelineInfo, layout);
	return rv_try_vkr(vkCreateGraphicsPipelines(device.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline));
}

void rv::Pipeline::Release()
{
	if (device)
		release(pipeline, *device);
}

rv::Result rv::Pipeline::Create(const std::vector<std::reference_wrapper<Pipeline>>& pipelines, const Device& device, const std::vector<std::reference_wrapper<const PipelineLayout>>& layouts)
{
	rv_result;
	rif_assert(pipelines.size() == layouts.size());

	for (auto& p : pipelines)
	{
		p.get().Release();
		p.get().device = &device;
	}

	std::vector<VkGraphicsPipelineCreateInfo> createInfo(layouts.size());
	std::vector<VkPipeline> vkpipelines(pipelines.size());
	for (u32 i = 0; i < layouts.size(); ++i)
		FillCreateInfo(createInfo[i], layouts[i]);
	std::transform(pipelines.begin(), pipelines.end(), vkpipelines.begin(), [](const Pipeline& pipeline) { return pipeline.pipeline; });
	
	rif_try_vkr(vkCreateGraphicsPipelines(device.device, VK_NULL_HANDLE, (u32)createInfo.size(), createInfo.data(), nullptr, vkpipelines.data()));

	for (u32 i = 0; i < layouts.size(); ++i)
		pipelines[i].get().pipeline = vkpipelines[i];

	return result;
}

void rv::Pipeline::FillCreateInfo(VkGraphicsPipelineCreateInfo& info, const PipelineLayout& layout)
{
	info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.stageCount = (u32)layout.shaderStages.size();
	info.pStages = layout.shaderStages.data();
	info.pVertexInputState = &layout.vertexInput;
	info.pInputAssemblyState = &layout.inputAssembly;
	info.pViewportState = &layout.viewportState;
	info.pRasterizationState = &layout.rasterizer;
	info.pMultisampleState = &layout.multisampling;
	info.pColorBlendState = &layout.colorBlending;
	info.layout = layout.layout;
	info.renderPass = layout.pass;
	info.subpass = layout.subpass;
}

rv::PipelineLayoutDescriptor::operator size_t() const
{
	return hash;
}

size_t rv::PipelineLayoutDescriptor::rehash()
{
	hash = rv::hash(
		topology,
		lineWidth,
		cullMode,
		clockwise,
		blending,
		shaders //,
//		renderpass,
//		subpass
	);
	return hash;
}

rv::FullPipeline::FullPipeline(FullPipeline&& rhs) noexcept
	:
	pipeline(std::move(rhs.pipeline)),
	layout(std::move(rhs.layout))
{
}

rv::FullPipeline::~FullPipeline()
{
	Release();
}

rv::FullPipeline& rv::FullPipeline::operator=(FullPipeline&& rhs) noexcept
{
	pipeline = std::move(rhs.pipeline);
	layout = std::move(rhs.layout);
	return *this;
}

void rv::FullPipeline::Release()
{
	pipeline.Release();
	layout.Release();
}

#include <iostream>

rv::Result rv::FullPipeline::Create(FullPipeline& pipeline, const Device& device)
{
	rv_result;
	rv_rif(PipelineLayout::Create(pipeline.layout, device));
	return Pipeline::Create(pipeline.pipeline, device, pipeline.layout);
}
