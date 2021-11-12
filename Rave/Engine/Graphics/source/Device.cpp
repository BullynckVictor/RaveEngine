#include "Engine/Graphics/Device.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"
#include "Engine/Core/Logger.h"

rv::Result rv::Device::Create(
	Device& device, 
	const Instance& instance,
	const PhysicalDeviceFeatures& requirements,
	const DeviceRater& rater
)
{
	rv_result;

	rv_rif(PhysicalDevice::Create(device.physical, instance, requirements, rater));
	rv_log(str("Picked device \"", device.physical.properties.deviceName, "\""));

	return result;
}

rv::Result rv::PhysicalDevice::Create(
	PhysicalDevice& device, 
	const Instance& instance,
	const PhysicalDeviceFeatures& requirements,
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

bool rv::PhysicalDevice::Suitable(const PhysicalDeviceFeatures& requirements) const
{
	if (!device)
		return false;

	const VkBool32* features = reinterpret_cast<const VkBool32*>(&this->features);

	for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
		if (requirements[i])
			if (!features[i])
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

int rv::PhysicalDevice::Rate(const PhysicalDeviceFeatures& requirements, const DeviceRater& rater) const
{
	int score = 0;

	if (!Suitable(requirements))
		return 0;

	score += rater.GetTypeMultiplier(properties.deviceType);
	score += rater.Rate(properties.limits);

	const VkBool32* features = reinterpret_cast<const VkBool32*>(&this->features);

	for (u32 i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
		if (features[i])
			score += rater.GetFeatureMultiplierIndex(i);

	return score;
}

VkBool32 rv::PhysicalDeviceFeatures::operator[](size_t index) const
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

rv::PhysicalDeviceFeatures rv::DefaultDeviceFeatures()
{
	PhysicalDeviceFeatures features;

	features.features.geometryShader = VK_TRUE;

	return features;
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
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupCount));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupInvocations));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxComputeWorkGroupSize));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, subPixelPrecisionBits));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, subTexelPrecisionBits));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, mipmapPrecisionBits));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDrawIndexedIndexValue));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxDrawIndirectCount));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxSamplerLodBias));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxSamplerAnisotropy));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxViewports));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, maxViewportDimensions));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, viewportBoundsRange));
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
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, pointSizeRange));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, lineWidthRange));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, pointSizeGranularity));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, lineWidthGranularity));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, strictLines));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, standardSampleLocations));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, optimalBufferCopyOffsetAlignment));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, optimalBufferCopyRowPitchAlignment));
	score += GetLimitMultiplier(offsetof(VkPhysicalDeviceLimits, nonCoherentAtomSize));

	return score;
}