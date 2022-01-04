#include "Engine/Graphics/DescriptorSet.h"
#include "Engine/Utility/Error.h"

template<>
void rv::destroy(VkDescriptorSetLayout layout, VkDevice device, VkInstance)
{
	vkDestroyDescriptorSetLayout(device, layout, nullptr);
}

template<>
void rv::destroy(VkDescriptorPool pool, VkDevice device, VkInstance)
{
	vkDestroyDescriptorPool(device, pool, nullptr);
}

void rv::DescriptorSetBindings::AddBinding(Flags<ShaderType, u32> shaderTypes, VkDescriptorType type, u32 count)
{
	VkDescriptorSetLayoutBinding binding{};
	binding.binding = (u32)bindings.size();
	binding.descriptorCount = count;
	binding.descriptorType = type;
	binding.stageFlags = shaderTypes.data();
	bindings.push_back(binding);
}

int rv::DescriptorSetBindings::operator<=>(const DescriptorSetBindings& rhs) const
{
	if (bindings.size() < rhs.bindings.size())
		return -1;
	else if (bindings.size() > rhs.bindings.size())
		return 1;
	else
		return memcmp(bindings.data(), rhs.bindings.data(), bindings.size() * sizeof(VkDescriptorSetLayoutBinding));
}

rv::DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& rhs) noexcept
	:
	layout(move(rhs.layout)),
	device(move(rhs.device))
{
}

rv::DescriptorSetLayout::~DescriptorSetLayout()
{
	Release();
}

rv::DescriptorSetLayout& rv::DescriptorSetLayout::operator=(DescriptorSetLayout&& rhs) noexcept
{
	layout = move(rhs.layout);
	device = move(rhs.device);
	return *this;
}

rv::Result rv::DescriptorSetLayout::Create(DescriptorSetLayout& layout, const Device& device, const DescriptorSetBindings& bindings)
{
	layout.Release();
	layout.device = &device;

	VkDescriptorSetLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = (u32)bindings.bindings.size();
	createInfo.pBindings = bindings.bindings.data();

	return rv_try_vkr(vkCreateDescriptorSetLayout(device.device, &createInfo, nullptr, &layout.layout));
}

void rv::DescriptorSetLayout::Release()
{
	if (device)
		release(layout, *device);
	device = nullptr;
}

void rv::DescriptorPoolSizes::AddSize(VkDescriptorType type, u32 size)
{
	sizes[type] += size;
}

rv::DescriptorPool::DescriptorPool(DescriptorPool&& rhs) noexcept
	:
	pool(move(rhs.pool)),
	device(move(rhs.device))
{
}

rv::DescriptorPool::~DescriptorPool()
{
	Release();
}

rv::DescriptorPool& rv::DescriptorPool::operator=(DescriptorPool&& rhs) noexcept
{
	pool = move(rhs.pool);
	device = move(rhs.device);
	return *this;
}

rv::Result rv::DescriptorPool::Create(DescriptorPool& pool, const Device& device, const DescriptorPoolSizes& sizes)
{
	pool.Release();
	pool.device = &device;

	u32 maxSize = 0;
	std::vector<VkDescriptorPoolSize> poolSizes;
	poolSizes.reserve(sizes.sizes.size());
	for (const auto& [type, size] : sizes.sizes)
	{
		VkDescriptorPoolSize dps{};
		dps.type = type;
		dps.descriptorCount = size;
		poolSizes.push_back(dps);
		maxSize += size;
	}

	VkDescriptorPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = (u32)poolSizes.size();
	createInfo.pPoolSizes = poolSizes.data();
	createInfo.maxSets = maxSize;

	return rv_try_vkr(vkCreateDescriptorPool(device.device, &createInfo, nullptr, &pool.pool));
}

rv::Result rv::DescriptorPool::Allocate(DescriptorSet& set, const DescriptorSetLayout& layout)
{
	Free(set);
	set.device = device;

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &layout.layout;

	return rv_try_vkr(vkAllocateDescriptorSets(device->device, &allocInfo, &set.set));
}

rv::Result rv::DescriptorPool::Allocate(std::vector<std::reference_wrapper<DescriptorSet>>& sets, const DescriptorSetLayout& layout)
{
	rv_result;

	std::vector<VkDescriptorSet> dsets(sets.size());
	std::vector<VkDescriptorSetLayout> layouts(sets.size(), layout.layout);

	for (DescriptorSet& set : sets)
	{
		Free(set);
		set.device = device;
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = (u32)layouts.size();
	allocInfo.pSetLayouts = layouts.data();

	rif_try_vkr(vkAllocateDescriptorSets(device->device, &allocInfo, dsets.data()));

	for (size_t i = 0; i < sets.size(); ++i)
		sets[i].get().set = dsets[i];

	return result;
}

rv::Result rv::DescriptorPool::Allocate(std::vector<std::reference_wrapper<DescriptorSet>>& sets, const std::vector<std::reference_wrapper<const DescriptorSetLayout>>& layouts)
{
	rv_result;
	rif_assert(sets.size() == layouts.size());

	std::vector<VkDescriptorSet> dsets(sets.size());
	std::vector<VkDescriptorSetLayout> dlayouts(sets.size());

	for (DescriptorSet& set : sets)
	{
		Free(set);
		set.device = device;
	}

	std::transform(layouts.begin(), layouts.end(), dlayouts.begin(), [](const DescriptorSetLayout& layout) { return layout.layout; });

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = (u32)dlayouts.size();
	allocInfo.pSetLayouts = dlayouts.data();

	rif_try_vkr(vkAllocateDescriptorSets(device->device, &allocInfo, dsets.data()));

	for (size_t i = 0; i < sets.size(); ++i)
		sets[i].get().set = dsets[i];

	return result;
}

rv::Result rv::DescriptorPool::Free(DescriptorSet& set)
{
	if (set.set)
	{
		Result result = rv_try_vkr(vkFreeDescriptorSets(device->device, pool, 1, &set.set));
		set.Release();
		return result;
	}
	return success;
}

rv::Result rv::DescriptorPool::Free(std::vector<std::reference_wrapper<DescriptorSet>>& sets)
{
	rv_result;

	if (std::any_of(sets.begin(), sets.end(), [](const DescriptorSet& set) { return set.set; }))
	{
		if (std::all_of(sets.begin(), sets.end(), [](const DescriptorSet& set) { return set.set; }))
		{
			std::vector<VkDescriptorSet> dsets(sets.size());
			result = rv_try_vkr(vkFreeDescriptorSets(device->device, pool, (u32)sets.size(), dsets.data()));

			for (DescriptorSet& set : sets)
				set.Release();
		}
		else
		{
			for (DescriptorSet& set : sets)
				rv_rif(Free(set));
		}
	}

	return result;
}

void rv::DescriptorPool::Release()
{
	if (device)
		release(pool, *device);
	device = nullptr;
}

rv::DescriptorSet::DescriptorSet(DescriptorSet&& rhs) noexcept
	:
	set(move(rhs.set)),
	device(move(rhs.device))
{
}

rv::DescriptorSet& rv::DescriptorSet::operator=(DescriptorSet&& rhs) noexcept
{
	set = move(rhs.set);
	device = move(rhs.device);
	return *this;
}

void rv::DescriptorSet::Write(VkDescriptorType type, const Buffer& buffer, u64 size, u64 offset, u32 binding, u32 index)
{
	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = buffer.buffer;
	bufferInfo.offset = offset;
	bufferInfo.range = size;

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = set;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = index;
	descriptorWrite.descriptorType = type;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(device->device, 1, &descriptorWrite, 0, nullptr);
}

void rv::DescriptorSet::Release()
{
	set = nullptr;
	device = nullptr;
}

rv::DescriptorSetAllocator::DescriptorSetAllocator(Device& device)
	:
	device(&device)
{
}

void rv::DescriptorSetAllocator::SetDevice(Device& device)
{
	this->device = &device;
}

rv::Result rv::DescriptorSetAllocator::GetQueue(Queue*& queue, const DescriptorSetBindings& bindings)
{
	rv_result;
	rif_assert(device);

	auto it = queues.find(bindings);
	if (it != queues.end())
	{
		queue = &it->second;
		return success;
	}

	queue = nullptr;

	auto& q = queues[bindings];
	rv_rif(DescriptorSetLayout::Create(q.layout, *device, bindings));

	for (const auto& binding : bindings.bindings)
		q.entrySizes.AddSize(binding.descriptorType, binding.descriptorCount);

	queue = &q;
	return result;
}

rv::Result rv::DescriptorSetAllocator::Allocate(DescriptorSet& set, const DescriptorSetBindings& bindings)
{
	rv_result;
	rif_assert(device);
	Queue* queue = nullptr;
	rv_rif(GetQueue(queue, bindings));
	return Allocate(set, *queue);
}

rv::Result rv::DescriptorSetAllocator::Allocate(DescriptorSet& set, Queue& queue)
{
	rv_result;
	rif_assert(device);

	DescriptorPool* pool = nullptr;
	rv_rif(queue.NextPool(pool, *device));
	rif_assert(pool);

	return pool->Allocate(set, queue.layout);
}

rv::Result rv::DescriptorSetAllocator::Queue::NextPool(DescriptorPool*& pool, Device& device)
{
	rv_result;
	pool = nullptr;
	if (!entries.empty())
	{
		Entry& entry = entries.back();
		if (entry.used < entry.max)
		{
			entry.used++;
			pool = &entry.pool;
			return result;
		}
	}

	Entry entry;
	entry.max = size;
	size *= 2;
	DescriptorPoolSizes sizes = entrySizes;
	for (auto& [type, size] : sizes.sizes)
		size *= entry.max;
	rv_rif(DescriptorPool::Create(entry.pool, device, entrySizes));
	entries.push(std::move(entry));
	pool = &entries.back().pool;
	return result;
}