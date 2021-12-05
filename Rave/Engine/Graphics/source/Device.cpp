#include "Engine/Graphics/Device.h"
#include "Engine/Graphics/SwapChain.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"
#include "Engine/Core/Logger.h"
#include <set>

template<>
void rv::destroy(VkDevice device, VkDevice, VkInstance instance)
{
	vkDestroyDevice(device, nullptr);
}

rv::Device::Device(Device&& rhs) noexcept
	:
	device(move(rhs.device)),
	physical(std::move(rhs.physical)),
	graphicsQueue(std::move(rhs.graphicsQueue)),
	computeQueue(std::move(rhs.computeQueue))
{
}

rv::Device::~Device()
{
	Release();
}

rv::Device& rv::Device::operator=(Device&& rhs) noexcept
{
	device = move(rhs.device);
	physical = std::move(rhs.physical);
	return *this;
}

void rv::Device::Release()
{
	physical.Release();
	release(device);
}

rv::Result rv::Device::Create(
	Device& device, 
	const Instance& instance,
	const PhysicalDeviceRequirements& requirements,
	const DeviceRater& rater
)
{
	device.Release();

	rv_result;
	rv_rif(PhysicalDevice::Create(device.physical, instance, requirements, rater));

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	{
		auto f = device.physical.GetQueueFamilies();

		std::set<u32> families;
		for (const auto& family : requirements.requiredFamilies)
		{
			OIndex32 index = f.GetFamily(family);
			rif_assert_info(index.valid(), "Device Queue Family not found");
			families.insert(index.value);
		}
		for (const auto& family : rater.queueFamilyMultipliers)
		{
			OIndex32 index = f.GetFamily(family.first);
			rif_assert_info(index.valid(), "Device Queue Family not found");
			families.insert(index.value);
		}

		queueCreateInfos.resize(families.size());

		size_t i = 0;
		for (const auto& family : families)
		{
			float queuePriority = 1.0f;
			queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos[i].queueFamilyIndex = family;
			queueCreateInfos[i].queueCount = 1;
			queueCreateInfos[i].pQueuePriorities = &queuePriority;
			++i;
		}
	}

	VkPhysicalDeviceFeatures features{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = (u32)queueCreateInfos.size();
	createInfo.pEnabledFeatures = &features;
	createInfo.enabledExtensionCount = (u32)requirements.extensions.extensions.size();
	createInfo.ppEnabledExtensionNames = requirements.extensions.extensions.data();

	rif_try_vkr(vkCreateDevice(device.physical.device, &createInfo, nullptr, &device.device));
	rv_log(str("Created device \"", device.physical.properties.deviceName, "\" with score ", device.physical.Rate(requirements, rater)));

	device.graphicsQueue = device.GetQueue(graphicsFamilyGetter);
	device.computeQueue = device.GetQueue(computeFamilyGetter);

	return result;
}

rv::Queue rv::Device::GetQueue(u32 family, u32 index) const
{
	Queue queue;
	vkGetDeviceQueue(device, family, index, &queue.queue);
	queue.family = family;
	queue.index = index;
	return queue;
}

rv::Queue rv::Device::GetQueue(const QueueFamilyGetter& family, u32 index) const
{
	Queue queue;
	OIndex32 f = physical.GetQueueFamilies().GetFamily(family);
	if (f.valid())
	{
		vkGetDeviceQueue(device, f.value, index, &queue.queue);
		queue.family = f.value;
		queue.index = index;
	}
	return queue;
}

rv::Result rv::PhysicalDevice::Create(
	PhysicalDevice& device, 
	const Instance& instance,
	const PhysicalDeviceRequirements& requirements,
	const DeviceRater& rater
)
{
	device.Release();

	rv_result;

	std::vector<PhysicalDevice> devices;
	rv_rif(Create(devices, instance));

	const PhysicalDevice* best = nullptr;
	int bestRating = 0;

	for (const auto& d : devices) 
	{
		int rating = d.Rate(requirements, rater);
		if (rating > bestRating) 
		{
			best = &d;
			bestRating = rating;
		}
	}
	rif_check_info(best, "Failed to find GPUs with Vulkan support");

	if (best)
		device = *best;

	return result;
}

#include <iostream>

rv::Result rv::PhysicalDevice::Create(std::vector<PhysicalDevice>& devices, const Instance& instance)
{
	rv_result;

	uint32_t deviceCount;
	rif_try_vkr(vkEnumeratePhysicalDevices(instance.instance, &deviceCount, nullptr));
	rif_check_info(deviceCount, "Failed to find GPUs with Vulkan support");
	devices.clear();
	devices.resize(deviceCount);
	{
		std::vector<VkPhysicalDevice> pdevices(deviceCount);
		rif_try_vkr(vkEnumeratePhysicalDevices(instance.instance, &deviceCount, pdevices.data()));
		for (size_t i = 0; i < deviceCount; ++i)
		{
			devices[i].device = pdevices[i];
			devices[i].FillData();
		}
	}

	return result;
}

void rv::PhysicalDevice::Release()
{
	device = VK_NULL_HANDLE;
}

bool rv::PhysicalDevice::Suitable(const PhysicalDeviceRequirements& requirements) const
{
	if (!device)
		return false;

	const VkBool32* features = reinterpret_cast<const VkBool32*>(&this->features);

	for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
		if (requirements[i])
			if (!features[i])
				return false;

	auto families = GetQueueFamilies();

	for (const auto& getter : requirements.requiredFamilies)
		if (families.GetFamily(getter).invalid())
			return false;

	auto extensionsSupport = SupportsExtensions(requirements.extensions);
	return extensionsSupport.succeeded() && extensionsSupport.value;
}

void rv::PhysicalDevice::FillData()
{
	if (device)
	{
		vkGetPhysicalDeviceProperties(device, &properties);
		vkGetPhysicalDeviceFeatures(device, &features);
	}
}

int rv::PhysicalDevice::Rate(const PhysicalDeviceRequirements& requirements, const DeviceRater& rater) const
{
	int score = 0;

	if (!Suitable(requirements))
		return 0;

	score += rater.GetTypeMultiplier(properties.deviceType);
	score += rater.Rate(properties.limits);

	auto families = GetQueueFamilies();

	for (const auto& family : rater.queueFamilyMultipliers)
		if (families.GetFamily(family.first))
			score += family.second;

	const VkBool32* features = reinterpret_cast<const VkBool32*>(&this->features);

	for (u32 i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
		if (features[i])
			score += rater.GetFeatureMultiplierIndex(i);

	return score;
}

rv::ResultValue<bool> rv::PhysicalDevice::SupportsExtensions(const Extensions& extensions) const
{
	rv_result;

	uint32_t extensionCount = 0;
	rif_try_vkr(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr));

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	rif_try_vkr(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()));

	for (const char* extension : extensions.extensions)
	{
		bool supported = false;
		for (const auto& e : availableExtensions)
		{
			if (strcmp(extension, e.extensionName) == 0)
			{
				supported = true;
				break;
			}
		}
		if (!supported)
			return false;
	}
	return true;
}

rv::QueueFamilies rv::PhysicalDevice::GetQueueFamilies() const
{
	QueueFamilies families;
	families.device = device;

	u32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	families.families.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, families.families.data());

	return families;
}

VkBool32 rv::PhysicalDeviceRequirements::operator[](size_t index) const
{
	return reinterpret_cast<const VkBool32*>(&features)[index];
}

rv::Queue::Queue(Queue&& rhs) noexcept
	:
	queue(std::move(rhs.queue)),
	family(rhs.family),
	index(rhs.index)
{
	rhs.family = 0;
	rhs.index = 0;
}

rv::Queue& rv::Queue::operator=(Queue&& rhs) noexcept
{
	queue = move(rhs.queue);
	family = rhs.family;
	index = rhs.index;
	rhs.family = 0;
	rhs.index = 0;
	return *this;
}

void rv::Queue::Release()
{
	queue = VK_NULL_HANDLE;
	family = 0;
	index = 0;
}

rv::Result rv::Queue::Wait() const
{
	return rv_try_vkr(vkQueueWaitIdle(queue));
}

void rv::DeviceRater::AddTypeMultiplier(VkPhysicalDeviceType type, int multiplier)
{
	typeMultipliers[type] = multiplier;
}

int rv::DeviceRater::GetTypeMultiplier(VkPhysicalDeviceType type) const
{
	return typeMultipliers[type];
}

void rv::DeviceRater::AddLimitMultiplier(u32 offset, float multiplier)
{
	limitMultiplier[offset] = multiplier;
}

float rv::DeviceRater::GetLimitMultiplier(u32 offset) const
{
	auto it = limitMultiplier.find(offset);
	if (it == limitMultiplier.end())
		return 0;
	return it->second;
}

void rv::DeviceRater::AddFeatureMultiplier(u32 offset, int multiplier)
{
	AddFeatureMultiplierIndex(offset / sizeof(VkBool32), multiplier);
}

void rv::DeviceRater::AddFeatureMultiplierIndex(u32 index, int multiplier)
{
	featureMultipliers[index] = multiplier;
}

int rv::DeviceRater::GetFeatureMultiplier(u32 offset) const
{
	return GetFeatureMultiplierIndex(offset / sizeof(VkBool32));
}

int rv::DeviceRater::GetFeatureMultiplierIndex(u32 index) const
{
	return featureMultipliers[index];
}

void rv::DeviceRater::AddQueueFamilyMultipler(const QueueFamilyGetter& family, int multiplier)
{
	queueFamilyMultipliers.emplace_back( family, multiplier );
}

int rv::DeviceRater::GetQueueFamilyMultipler(const QueueFamilyGetter& family) const
{
	for (const auto& f : queueFamilyMultipliers)
		if (f.first == family)
			return f.second;
	return 0;
}

rv::PhysicalDeviceRequirements rv::DefaultDeviceRequirements(const std::vector<std::reference_wrapper<const Surface>>& surfaces)
{
	PhysicalDeviceRequirements requirements;

	requirements.features.geometryShader = VK_TRUE;
	requirements.requiredFamilies.push_back(graphicsFamilyGetter);
	for (auto surface : surfaces)
		requirements.requiredFamilies.emplace_back(RV_QUEUE_GETTER_TYPE_PRESENT, surface.get().surface);

	requirements.extensions.AddExtension(RV_EXTENSION_SWAPCHAIN);

	return requirements;
}

rv::OIndex32 rv::QueueFamilies::GetFamily(QueueFamilyGetter getter) const
{
	u32 i = 0;
	for (const auto& family : families)
	{
		auto isFamily = getter.IsFamily(device, family, i);
		if (isFamily.failed())
			return {};
		if (isFamily.value)
			return i;
		i++;
	}
	return {};
}


rv::ResultValue<bool> rv::QueueFamilyGetter::IsFamily(VkPhysicalDevice device, const VkQueueFamilyProperties& family, u32 index) const
{
	rv_result;
	switch (type)
	{
	case RV_QUEUE_GETTER_TYPE_FLAGS:
		return family.queueFlags & data.flags;

	case RV_QUEUE_GETTER_TYPE_PRESENT:
		VkBool32 presentSupport = false;
		rif_try_vkr(vkGetPhysicalDeviceSurfaceSupportKHR(device, index, data.surface, &presentSupport));
		return presentSupport;
	}
	return false;
}

rv::DeviceRater rv::DefaultDeviceRater()
{
	DeviceRater rater;

	VkPhysicalDeviceLimits* limits = nullptr;

	rater.AddTypeMultiplier(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, 4000);
	rater.AddLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxImageDimension2D), 0.1f);

	return rater;
}

int rv::DeviceRater::Rate(const VkPhysicalDeviceLimits& limits) const
{
	int score = 0;

	score += (int)((float)limits.maxImageDimension1D									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxImageDimension1D)));
	score += (int)((float)limits.maxImageDimension2D									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxImageDimension2D)));
	score += (int)((float)limits.maxImageDimension3D									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxImageDimension3D)));
	score += (int)((float)limits.maxImageDimensionCube									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxImageDimensionCube)));
	score += (int)((float)limits.maxImageArrayLayers									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxImageArrayLayers)));
	score += (int)((float)limits.maxTexelBufferElements									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTexelBufferElements)));
	score += (int)((float)limits.maxUniformBufferRange									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxUniformBufferRange)));
	score += (int)((float)limits.maxStorageBufferRange									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxStorageBufferRange)));
	score += (int)((float)limits.maxPushConstantsSize									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPushConstantsSize)));
	score += (int)((float)limits.maxMemoryAllocationCount								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxMemoryAllocationCount)));
	score += (int)((float)limits.maxSamplerAllocationCount								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxSamplerAllocationCount)));
	score += (int)((float)limits.bufferImageGranularity									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, bufferImageGranularity)));
	score += (int)((float)limits.sparseAddressSpaceSize									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, sparseAddressSpaceSize)));
	score += (int)((float)limits.maxBoundDescriptorSets									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxBoundDescriptorSets)));
	score += (int)((float)limits.maxPerStageDescriptorSamplers							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageDescriptorSamplers)));
	score += (int)((float)limits.maxPerStageDescriptorUniformBuffers					*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageDescriptorUniformBuffers)));
	score += (int)((float)limits.maxPerStageDescriptorStorageBuffers					*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageDescriptorStorageBuffers)));
	score += (int)((float)limits.maxPerStageDescriptorSampledImages						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageDescriptorSampledImages)));
	score += (int)((float)limits.maxPerStageDescriptorStorageImages						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageDescriptorStorageImages)));
	score += (int)((float)limits.maxPerStageDescriptorInputAttachments					*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageDescriptorInputAttachments)));
	score += (int)((float)limits.maxPerStageResources									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageResources)));
	score += (int)((float)limits.maxDescriptorSetSamplers								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetSamplers)));
	score += (int)((float)limits.maxDescriptorSetUniformBuffers							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetUniformBuffers)));
	score += (int)((float)limits.maxDescriptorSetUniformBuffersDynamic					*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetUniformBuffersDynamic)));
	score += (int)((float)limits.maxDescriptorSetStorageBuffers							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetStorageBuffers)));
	score += (int)((float)limits.maxDescriptorSetStorageBuffersDynamic					*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetStorageBuffersDynamic)));
	score += (int)((float)limits.maxDescriptorSetSampledImages							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetSampledImages)));
	score += (int)((float)limits.maxDescriptorSetStorageImages							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetStorageImages)));
	score += (int)((float)limits.maxDescriptorSetInputAttachments						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetInputAttachments)));
	score += (int)((float)limits.maxVertexInputAttributes								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxVertexInputAttributes)));
	score += (int)((float)limits.maxVertexInputBindings									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxVertexInputBindings)));
	score += (int)((float)limits.maxVertexInputAttributeOffset							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxVertexInputAttributeOffset)));
	score += (int)((float)limits.maxVertexInputBindingStride							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxVertexInputBindingStride)));
	score += (int)((float)limits.maxVertexOutputComponents								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxVertexOutputComponents)));
	score += (int)((float)limits.maxTessellationGenerationLevel							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationGenerationLevel)));
	score += (int)((float)limits.maxTessellationPatchSize								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationPatchSize)));
	score += (int)((float)limits.maxTessellationControlPerVertexInputComponents			*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationControlPerVertexInputComponents)));
	score += (int)((float)limits.maxTessellationControlPerVertexOutputComponents		*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationControlPerVertexOutputComponents)));
	score += (int)((float)limits.maxTessellationControlPerPatchOutputComponents			*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationControlPerPatchOutputComponents)));
	score += (int)((float)limits.maxTessellationControlTotalOutputComponents			*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationControlTotalOutputComponents)));
	score += (int)((float)limits.maxTessellationEvaluationInputComponents				*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationEvaluationInputComponents)));
	score += (int)((float)limits.maxTessellationEvaluationOutputComponents				*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationEvaluationOutputComponents)));
	score += (int)((float)limits.maxGeometryShaderInvocations							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxGeometryShaderInvocations)));
	score += (int)((float)limits.maxGeometryInputComponents								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxGeometryInputComponents)));
	score += (int)((float)limits.maxGeometryOutputComponents							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxGeometryOutputComponents)));
	score += (int)((float)limits.maxGeometryOutputVertices								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxGeometryOutputVertices)));
	score += (int)((float)limits.maxGeometryTotalOutputComponents						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxGeometryTotalOutputComponents)));
	score += (int)((float)limits.maxFragmentInputComponents								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFragmentInputComponents)));
	score += (int)((float)limits.maxFragmentOutputAttachments							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFragmentOutputAttachments)));
	score += (int)((float)limits.maxFragmentDualSrcAttachments							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFragmentDualSrcAttachments)));
	score += (int)((float)limits.maxFragmentCombinedOutputResources						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFragmentCombinedOutputResources)));
	score += (int)((float)limits.maxComputeSharedMemorySize								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeSharedMemorySize)));
	score += (int)((float)limits.maxComputeWorkGroupCount[0]							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupCount)));
	score += (int)((float)limits.maxComputeWorkGroupCount[1]							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupCount)));
	score += (int)((float)limits.maxComputeWorkGroupCount[2]							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupCount)));
	score += (int)((float)limits.maxComputeWorkGroupInvocations							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupInvocations)));
	score += (int)((float)limits.maxComputeWorkGroupSize[0]								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupSize)));
	score += (int)((float)limits.maxComputeWorkGroupSize[1]								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupSize)));
	score += (int)((float)limits.maxComputeWorkGroupSize[2]								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupSize)));
	score += (int)((float)limits.subPixelPrecisionBits									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, subPixelPrecisionBits)));
	score += (int)((float)limits.subTexelPrecisionBits									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, subTexelPrecisionBits)));
	score += (int)((float)limits.mipmapPrecisionBits									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, mipmapPrecisionBits)));
	score += (int)((float)limits.maxDrawIndexedIndexValue								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDrawIndexedIndexValue)));
	score += (int)((float)limits.maxDrawIndirectCount									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDrawIndirectCount)));
	score += (int)((float)limits.maxSamplerLodBias										*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxSamplerLodBias)));
	score += (int)((float)limits.maxSamplerAnisotropy									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxSamplerAnisotropy)));
	score += (int)((float)limits.maxViewports											*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxViewports)));
	score += (int)((float)limits.maxViewportDimensions[0]								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxViewportDimensions)));
	score += (int)((float)limits.maxViewportDimensions[1]								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxViewportDimensions)));
	score += (int)((float)limits.viewportBoundsRange[0]									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, viewportBoundsRange)));
	score += (int)((float)limits.viewportBoundsRange[1]									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, viewportBoundsRange)));
	score += (int)((float)limits.viewportSubPixelBits									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, viewportSubPixelBits)));
	score += (int)((float)limits.minMemoryMapAlignment									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minMemoryMapAlignment)));
	score += (int)((float)limits.minTexelBufferOffsetAlignment							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minTexelBufferOffsetAlignment)));
	score += (int)((float)limits.minUniformBufferOffsetAlignment						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minUniformBufferOffsetAlignment)));
	score += (int)((float)limits.minStorageBufferOffsetAlignment						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minStorageBufferOffsetAlignment)));
	score += (int)((float)limits.minTexelOffset											*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minTexelOffset)));
	score += (int)((float)limits.maxTexelOffset											*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTexelOffset)));
	score += (int)((float)limits.minTexelGatherOffset									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minTexelGatherOffset)));
	score += (int)((float)limits.maxTexelGatherOffset									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTexelGatherOffset)));
	score += (int)((float)limits.minInterpolationOffset									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minInterpolationOffset)));
	score += (int)((float)limits.maxInterpolationOffset									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxInterpolationOffset)));
	score += (int)((float)limits.subPixelInterpolationOffsetBits						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, subPixelInterpolationOffsetBits)));
	score += (int)((float)limits.maxFramebufferWidth									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFramebufferWidth)));
	score += (int)((float)limits.maxFramebufferHeight									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFramebufferHeight)));
	score += (int)((float)limits.maxFramebufferLayers									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFramebufferLayers)));
	score += (int)((float)limits.framebufferColorSampleCounts							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, framebufferColorSampleCounts)));
	score += (int)((float)limits.framebufferDepthSampleCounts							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, framebufferDepthSampleCounts)));
	score += (int)((float)limits.framebufferStencilSampleCounts							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, framebufferStencilSampleCounts)));
	score += (int)((float)limits.framebufferNoAttachmentsSampleCounts					*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, framebufferNoAttachmentsSampleCounts)));
	score += (int)((float)limits.maxColorAttachments									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxColorAttachments)));
	score += (int)((float)limits.sampledImageColorSampleCounts							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, sampledImageColorSampleCounts)));
	score += (int)((float)limits.sampledImageIntegerSampleCounts						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, sampledImageIntegerSampleCounts)));
	score += (int)((float)limits.sampledImageDepthSampleCounts							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, sampledImageDepthSampleCounts)));
	score += (int)((float)limits.sampledImageStencilSampleCounts						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, sampledImageStencilSampleCounts)));
	score += (int)((float)limits.storageImageSampleCounts								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, storageImageSampleCounts)));
	score += (int)((float)limits.maxSampleMaskWords										*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxSampleMaskWords)));
	score += (int)((float)limits.timestampComputeAndGraphics							*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, timestampComputeAndGraphics)));
	score += (int)((float)limits.timestampPeriod										*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, timestampPeriod)));
	score += (int)((float)limits.maxClipDistances										*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxClipDistances)));
	score += (int)((float)limits.maxCullDistances										*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxCullDistances)));
	score += (int)((float)limits.maxCombinedClipAndCullDistances						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxCombinedClipAndCullDistances)));
	score += (int)((float)limits.discreteQueuePriorities								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, discreteQueuePriorities)));
	score += (int)((float)limits.pointSizeRange[0]										*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, pointSizeRange)));
	score += (int)((float)limits.pointSizeRange[1]										*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, pointSizeRange)));
	score += (int)((float)limits.lineWidthRange[0]										*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, lineWidthRange)));
	score += (int)((float)limits.lineWidthRange[1]										*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, lineWidthRange)));
	score += (int)((float)limits.pointSizeGranularity									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, pointSizeGranularity)));
	score += (int)((float)limits.lineWidthGranularity									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, lineWidthGranularity)));
	score += (int)((float)limits.strictLines											*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, strictLines)));
	score += (int)((float)limits.standardSampleLocations								*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, standardSampleLocations)));
	score += (int)((float)limits.optimalBufferCopyOffsetAlignment						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, optimalBufferCopyOffsetAlignment)));
	score += (int)((float)limits.optimalBufferCopyRowPitchAlignment						*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, optimalBufferCopyRowPitchAlignment)));
	score += (int)((float)limits.nonCoherentAtomSize									*	GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, nonCoherentAtomSize)));

	return score;
}