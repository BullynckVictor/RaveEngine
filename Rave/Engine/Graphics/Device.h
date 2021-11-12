#pragma once
#include "Engine/Graphics/Instance.h"
#include <array>
#include <map>

namespace rv
{
	struct PhysicalDeviceFeatures
	{
		PhysicalDeviceFeatures() = default;

		VkBool32 operator[] (size_t index) const;

		VkPhysicalDeviceFeatures features = {};
	};

	struct DeviceRater
	{
		DeviceRater() = default;

		void AddTypeMultiplier(VkPhysicalDeviceType type, int multiplier = 1);
		int GetTypeMultiplier(VkPhysicalDeviceType type) const;

		void AddLimitMultiplier(u32 offset, int multiplier = 1);
		int GetLimitMultiplier(u32 offset) const;

		void AddFeatureMultiplier(u32 offset, int multiplier = 1);
		void AddFeatureMultiplierIndex(u32 index, int multiplier = 1);
		int GetFeatureMultiplier(u32 offset) const;
		int GetFeatureMultiplierIndex(u32 index) const;

		int Rate(const VkPhysicalDeviceLimits& limits) const;

		std::array<int, VK_PHYSICAL_DEVICE_TYPE_CPU + 1> typeMultipliers;
		std::map<int, u32> limitMultiplier;
		std::array<int, sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32)> featureMultipliers;
	};

	PhysicalDeviceFeatures DefaultDeviceFeatures();
	DeviceRater DefaultDeviceRater();

	struct PhysicalDevice
	{
		PhysicalDevice() = default;

		static Result Create(
			PhysicalDevice& device,
			const Instance& instance,
			const PhysicalDeviceFeatures& requirements,
			const DeviceRater& rater
		);
		static Result Create(std::vector<PhysicalDevice>& devices, const Instance& instance);

		bool Suitable(const PhysicalDeviceFeatures& requirements) const;
		void FillData();
		int Rate(const PhysicalDeviceFeatures& requirements, const DeviceRater& rater) const;

		VkPhysicalDevice device = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties properties = {};
		VkPhysicalDeviceFeatures features = {};
	};

	struct Device
	{
		Device() = default;

		static Result Create(
			Device& device,
			const Instance& instance,
			const PhysicalDeviceFeatures& requirements = DefaultDeviceFeatures(),
			const DeviceRater& rater = DefaultDeviceRater()
		);

		VkDevice device = VK_NULL_HANDLE;
		PhysicalDevice physical;
	};
}