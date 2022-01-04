#pragma once
#include "Engine/Graphics/Device.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Graphics/Buffer.h"
#include <map>
#include <queue>

namespace rv
{
	struct DescriptorSetBindings
	{
		DescriptorSetBindings() = default;

		void AddBinding(Flags<ShaderType, u32> shaderTypes, VkDescriptorType type, u32 count = 1);

		int operator<=> (const DescriptorSetBindings& rhs) const;

		std::vector<VkDescriptorSetLayoutBinding> bindings;
	};

	struct DescriptorSetLayout
	{
		DescriptorSetLayout() = default;
		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept;
		~DescriptorSetLayout();

		DescriptorSetLayout& operator= (const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator= (DescriptorSetLayout&& rhs) noexcept;

		static Result Create(DescriptorSetLayout& layout, const Device& device, const DescriptorSetBindings& bindings);

		void Release();

		VkDescriptorSetLayout layout = VK_NULL_HANDLE;
		const Device* device = nullptr;
	};

	struct DescriptorPoolSizes
	{
		DescriptorPoolSizes() = default;

		void AddSize(VkDescriptorType type, u32 size);

		std::map<VkDescriptorType, u32> sizes;
	};

	struct DescriptorSet;

	struct DescriptorPool
	{
		DescriptorPool() = default;
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool(DescriptorPool&& rhs) noexcept;
		~DescriptorPool();

		DescriptorPool& operator= (const DescriptorPool&) = delete;
		DescriptorPool& operator= (DescriptorPool&& rhs) noexcept;

		static Result Create(DescriptorPool& pool, const Device& device, const DescriptorPoolSizes& sizes);

		Result Allocate(DescriptorSet& set, const DescriptorSetLayout& layout);
		Result Allocate(std::vector<std::reference_wrapper<DescriptorSet>>& sets, const DescriptorSetLayout& layout);
		Result Allocate(std::vector<std::reference_wrapper<DescriptorSet>>& sets, const std::vector<std::reference_wrapper<const DescriptorSetLayout>>& layouts);

		Result Free(DescriptorSet& set);
		Result Free(std::vector<std::reference_wrapper<DescriptorSet>>& sets);

		void Release();

		VkDescriptorPool pool = VK_NULL_HANDLE;
		const Device* device = nullptr;
	};

	struct DescriptorSet
	{
		DescriptorSet() = default;
		DescriptorSet(const DescriptorSet&) = delete;
		DescriptorSet(DescriptorSet&& rhs) noexcept;

		DescriptorSet& operator= (const DescriptorSet&) = delete;
		DescriptorSet& operator= (DescriptorSet&& rhs) noexcept;

		void Write(VkDescriptorType type, const Buffer& buffer, u64 size, u64 offset, u32 binding, u32 index = 0);

		void Release();

		VkDescriptorSet set = VK_NULL_HANDLE;
		const Device* device = nullptr;
	};

	class DescriptorSetAllocator
	{
	public:
		DescriptorSetAllocator() = default;
		DescriptorSetAllocator(Device& device);

		void SetDevice(Device& device);

	public:
		struct Entry
		{
			Entry() = default;

			DescriptorPool pool;
			u32 max = 0;
			u32 used = 0;
		};

		struct Queue
		{
			Queue() = default;

			Result NextPool(DescriptorPool*& pool, Device& device);

			DescriptorSetLayout layout;
			DescriptorPoolSizes entrySizes;
			std::queue<Entry> entries;
			u32 size = 0;
		};

		Result GetQueue(Queue*& queue, const DescriptorSetBindings& bindings);
		Result Allocate(DescriptorSet& set, const DescriptorSetBindings& bindings);
		Result Allocate(DescriptorSet& set, Queue& queue);

	private:
		std::map<DescriptorSetBindings, Queue> queues;
		Device* device = nullptr;
	};
}