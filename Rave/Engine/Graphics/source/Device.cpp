#include "Engine/Graphics/Device.h"
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
	physical(std::move(rhs.physical))
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

	rif_try_vkr(vkCreateDevice(device.physical.device, &createInfo, nullptr, &device.device));
	rv_log(str("Created device \"", device.physical.properties.deviceName, "\" with score ", device.physical.Rate(requirements, rater)));

	return result;
}

rv::Queue rv::Device::GetQueue(u32 family, u32 index) const
{
	Queue queue;
	vkGetDeviceQueue(device, family, index, &queue.queue);
	return queue;
}

rv::Queue rv::Device::GetQueue(const QueueFamilyGetter& family, u32 index) const
{
	Queue queue;
	OIndex32 f = physical.GetQueueFamilies().GetFamily(family);
	if (f.valid())
		vkGetDeviceQueue(device, f.value, index, &queue.queue);		
	return queue;
}

rv::Result rv::PhysicalDevice::Create(
	PhysicalDevice& device, 
	const Instance& instance,
	const PhysicalDeviceRequirements& requirements,
	const DeviceRater& rater
)
{
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

rv::Result rv::PhysicalDevice::Create(std::vector<PhysicalDevice>& devices, const Instance& instance)
{
	rv_result;

	uint32_t deviceCount = 0;
	rif_assert_vkr(vkEnumeratePhysicalDevices(instance.instance, &deviceCount, nullptr));
	rif_check_info(deviceCount, "Failed to find GPUs with Vulkan support");
	devices.clear();
	devices.resize(deviceCount);
	{
		std::vector<VkPhysicalDevice> pdevices(deviceCount);
		rif_assert_vkr(vkEnumeratePhysicalDevices(instance.instance, &deviceCount, pdevices.data()));
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

	return true;
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

rv::QueueFamilies rv::PhysicalDevice::GetQueueFamilies() const
{
	QueueFamilies families;

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

void rv::DeviceRater::AddTypeMultiplier(VkPhysicalDeviceType type, int multiplier)
{
	typeMultipliers[type] = multiplier;
}

int rv::DeviceRater::GetTypeMultiplier(VkPhysicalDeviceType type) const
{
	return typeMultipliers[type];
}

void rv::DeviceRater::AddLimitMultiplier(u32 offset, int multiplier)
{
	limitMultiplier[offset] = multiplier;
}

int rv::DeviceRater::GetLimitMultiplier(u32 offset) const
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

rv::PhysicalDeviceRequirements rv::DefaultDeviceRequirements()
{
	PhysicalDeviceRequirements requirements;

	requirements.features.geometryShader = VK_TRUE;
	requirements.requiredFamilies.push_back(graphicsFamilyGetter);

	return requirements;
}

rv::DeviceRater rv::DefaultDeviceRater()
{
	DeviceRater rater;

	VkPhysicalDeviceLimits* limits = nullptr;

	rater.AddTypeMultiplier(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU, 1000);
	rater.AddLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxImageDimension2D));

	return rater;
}

int rv::DeviceRater::Rate(const VkPhysicalDeviceLimits& limits) const
{
	int score = 0;

	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxImageDimension1D));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxImageDimension2D));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxImageDimension3D));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxImageDimensionCube));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxImageArrayLayers));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTexelBufferElements));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxUniformBufferRange));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxStorageBufferRange));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPushConstantsSize));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxMemoryAllocationCount));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxSamplerAllocationCount));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, bufferImageGranularity));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, sparseAddressSpaceSize));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxBoundDescriptorSets));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageDescriptorSamplers));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageDescriptorUniformBuffers));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageDescriptorStorageBuffers));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageDescriptorSampledImages));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageDescriptorStorageImages));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageDescriptorInputAttachments));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxPerStageResources));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetSamplers));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetUniformBuffers));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetUniformBuffersDynamic));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetStorageBuffers));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetStorageBuffersDynamic));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetSampledImages));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetStorageImages));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDescriptorSetInputAttachments));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxVertexInputAttributes));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxVertexInputBindings));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxVertexInputAttributeOffset));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxVertexInputBindingStride));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxVertexOutputComponents));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationGenerationLevel));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationPatchSize));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationControlPerVertexInputComponents));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationControlPerVertexOutputComponents));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationControlPerPatchOutputComponents));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationControlTotalOutputComponents));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationEvaluationInputComponents));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTessellationEvaluationOutputComponents));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxGeometryShaderInvocations));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxGeometryInputComponents));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxGeometryOutputComponents));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxGeometryOutputVertices));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxGeometryTotalOutputComponents));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFragmentInputComponents));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFragmentOutputAttachments));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFragmentDualSrcAttachments));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFragmentCombinedOutputResources));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeSharedMemorySize));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupCount[3]));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupInvocations));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupSize[3]));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, subPixelPrecisionBits));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, subTexelPrecisionBits));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, mipmapPrecisionBits));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDrawIndexedIndexValue));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDrawIndirectCount));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxSamplerLodBias));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxSamplerAnisotropy));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxViewports));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxViewportDimensions[2]));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, viewportBoundsRange[2]));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, viewportSubPixelBits));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minMemoryMapAlignment));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minTexelBufferOffsetAlignment));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minUniformBufferOffsetAlignment));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minStorageBufferOffsetAlignment));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minTexelOffset));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTexelOffset));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minTexelGatherOffset));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxTexelGatherOffset));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, minInterpolationOffset));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxInterpolationOffset));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, subPixelInterpolationOffsetBits));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFramebufferWidth));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFramebufferHeight));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxFramebufferLayers));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, framebufferColorSampleCounts));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, framebufferDepthSampleCounts));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, framebufferStencilSampleCounts));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, framebufferNoAttachmentsSampleCounts));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxColorAttachments));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, sampledImageColorSampleCounts));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, sampledImageIntegerSampleCounts));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, sampledImageDepthSampleCounts));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, sampledImageStencilSampleCounts));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, storageImageSampleCounts));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxSampleMaskWords));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, timestampComputeAndGraphics));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, timestampPeriod));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxClipDistances));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxCullDistances));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxCombinedClipAndCullDistances));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, discreteQueuePriorities));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, pointSizeRange[2]));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, lineWidthRange[2]));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, pointSizeGranularity));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, lineWidthGranularity));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, strictLines));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, standardSampleLocations));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, optimalBufferCopyOffsetAlignment));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, optimalBufferCopyRowPitchAlignment));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, nonCoherentAtomSize));

	return score;
}

rv::OIndex32 rv::QueueFamilies::GetFamily(QueueFamilyGetter getter) const
{
	u32 i = 0;
	for (const auto& family : families) 
	{
		if (getter.IsFamily(family))
			return i;
		i++;
	}
	return {};
}


bool rv::QueueFamilyGetter::IsFamily(const VkQueueFamilyProperties& family) const
{
	switch (type)
	{
		case RV_QUEUE_GETTER_TYPE_FLAGS:
			return family.queueFlags & data.flags;
	}
	return false;
}
