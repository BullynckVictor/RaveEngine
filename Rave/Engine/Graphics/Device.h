#pragma once
#include "Engine/Graphics/Instance.h"
#include "Engine/Utility/Optional.h"
#include <array>
#include <map>
#include <list>

namespace rv
{
	struct QueueFamilyGetter;

	struct PhysicalDeviceRequirements
	{
		PhysicalDeviceRequirements() = default;

		VkBool32 operator[] (size_t index) const;

		VkPhysicalDeviceFeatures features = {};
		std::list<QueueFamilyGetter> requiredFamilies;
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

		void AddQueueFamilyMultipler(const QueueFamilyGetter& family, int multiplier = 1);
		int GetQueueFamilyMultipler(const QueueFamilyGetter& family) const;

		int Rate(const VkPhysicalDeviceLimits& limits) const;

		std::array<int, VK_PHYSICAL_DEVICE_TYPE_CPU + 1> typeMultipliers = {};
		std::map<int, u32> limitMultiplier;
		std::array<int, sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32)> featureMultipliers = {};
		std::list<std::pair<QueueFamilyGetter, int>> queueFamilyMultipliers;
	};

	PhysicalDeviceRequirements DefaultDeviceRequirements();
	DeviceRater DefaultDeviceRater();

	struct Queue
	{
		Queue() = default;

		VkQueue queue = VK_NULL_HANDLE;
	};

	enum QueueGetterType
	{
		RV_QUEUE_GETTER_TYPE_FLAGS
	};

	struct QueueFamilyGetter
	{
		QueueGetterType type{};
		union Data
		{
			Data() = default;
			constexpr Data(VkQueueFlags flags)
				:
				flags(flags)
			{}

			VkQueueFlags flags;
		} data{};

		constexpr bool operator== (const QueueFamilyGetter& rhs) const
		{
			if (type != rhs.type)
				return false;
			switch (type)
			{
				case RV_QUEUE_GETTER_TYPE_FLAGS: return data.flags == rhs.data.flags;
			}
			return false;
		}

		constexpr QueueFamilyGetter() = default;
		constexpr QueueFamilyGetter(QueueGetterType type, Data data) 
			:
			type(type),
			data(data)
		{
		}

		bool IsFamily(const VkQueueFamilyProperties& family) const;
	};

	static constexpr QueueFamilyGetter graphicsFamilyGetter = QueueFamilyGetter(RV_QUEUE_GETTER_TYPE_FLAGS, VK_QUEUE_GRAPHICS_BIT);

	struct QueueFamilies
	{
		QueueFamilies() = default;

		OIndex32 GetFamily(QueueFamilyGetter getter) const;

		std::vector<VkQueueFamilyProperties> families;
	};

	struct PhysicalDevice
	{
		PhysicalDevice() = default;

		static Result Create(
			PhysicalDevice& device,
			const Instance& instance,
			const PhysicalDeviceRequirements& requirements,
			const DeviceRater& rater
		);
		static Result Create(std::vector<PhysicalDevice>& devices, const Instance& instance);

		void Release();

		bool Suitable(const PhysicalDeviceRequirements& requirements) const;
		void FillData();
		int Rate(const PhysicalDeviceRequirements& requirements, const DeviceRater& rater) const;

		QueueFamilies GetQueueFamilies() const;

		VkPhysicalDevice device = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties properties = {};
		VkPhysicalDeviceFeatures features = {};
	};

	struct Device
	{
		Device() = default;
		Device(const Device&) = delete;
		Device(Device&& rhs) noexcept;
		~Device();

		Device& operator= (const Device&) = delete;
		Device& operator= (Device&& rhs) noexcept;

		void Release();

		static Result Create(
			Device& device,
			const Instance& instance,
			const PhysicalDeviceRequirements& requirements = DefaultDeviceRequirements(),
			const DeviceRater& rater = DefaultDeviceRater()
		);

		Queue GetQueue(u32 family, u32 index = 0) const;
		Queue GetQueue(const QueueFamilyGetter& family, u32 index = 0) const;

		VkDevice device = VK_NULL_HANDLE;
		PhysicalDevice physical;
	};
}