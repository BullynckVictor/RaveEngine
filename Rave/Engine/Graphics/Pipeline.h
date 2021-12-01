#pragma once
#include "Engine/Graphics/Device.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/RenderPass.h"
#include <type_traits>

namespace rv
{
	struct PipelineLayoutDescriptor
	{
		PipelineLayoutDescriptor() = default;

		operator size_t () const;

		VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		float lineWidth = 1.0f;
		VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
		bool clockwise = true;
		bool blending = false;
		std::vector<const char*> shaders;
		const char* renderpass = nullptr;
		u32 subpass = 0;
		size_t hash = 0;

		size_t rehash();
	};

	struct PipelineLayout
	{
		PipelineLayout();
		PipelineLayout(PipelineLayout&) = delete;
		PipelineLayout(PipelineLayout&& rhs) noexcept;
		~PipelineLayout();

		PipelineLayout& operator= (const PipelineLayout&) = delete;
		PipelineLayout& operator= (PipelineLayout&& rhs) noexcept;

		static Result Create(PipelineLayout& layout, const Device& device);

		void Release();

		void SetToDescriptor(const PipelineLayoutDescriptor& descriptor);
		void SetTopology(VkPrimitiveTopology topology);
		void SetSize(const Extent2D& size);
		void SetLineWidth(float width);
		void SetCullMode(VkCullModeFlags cull);
		void SetCull(bool cull);
		void SetWinding(bool clockwise);
		void SetBlending(bool blend);
		void AddShader(const Shader& shader);
		void AddRenderPass(const RenderPass& pass, u32 subpass);

		VkPipelineLayout layout = VK_NULL_HANDLE;
		VkPipelineVertexInputStateCreateInfo vertexInput{};
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		VkViewport viewport{};
		VkRect2D scissor{};
		VkPipelineViewportStateCreateInfo viewportState{};
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		VkPipelineMultisampleStateCreateInfo multisampling{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		VkRenderPass pass = VK_NULL_HANDLE;
		u32 subpass = 0;
		const Device* device = nullptr;
	};

	struct Pipeline
	{
		Pipeline() = default;
		Pipeline(Pipeline&) = delete;
		Pipeline(Pipeline&& rhs) noexcept;
		~Pipeline();

		Pipeline& operator= (const Pipeline&) = delete;
		Pipeline& operator= (Pipeline&& rhs) noexcept;

		static Result Create(Pipeline& pipeline, const Device& device, const PipelineLayout& layout);
		static Result Create(
			const std::vector<std::reference_wrapper<Pipeline>>& pipelines,
			const Device& device,
			const std::vector<std::reference_wrapper<const PipelineLayout>>& layouts
		);

		static void FillCreateInfo(VkGraphicsPipelineCreateInfo& info, const PipelineLayout& layout);

		void Release();

		VkPipeline pipeline = VK_NULL_HANDLE;
		const Device* device = nullptr;
	};

	struct FullPipeline
	{
		FullPipeline() = default;

		FullPipeline(FullPipeline&) = delete;
		FullPipeline(FullPipeline&& rhs) noexcept;
		~FullPipeline();

		FullPipeline& operator= (const FullPipeline&) = delete;
		FullPipeline& operator= (FullPipeline&& rhs) noexcept;

		void Release();

		static Result Create(FullPipeline& pipeline, const Device& device);

		Pipeline pipeline;
		PipelineLayout layout;
	};
}