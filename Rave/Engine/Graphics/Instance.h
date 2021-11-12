#pragma once
#include "Engine/Graphics/Vulkan.h"
#include "Engine/Utility/Result.h"

namespace rv
{
	struct ApplicationInfo
	{
		ApplicationInfo();
		ApplicationInfo(const char* name, u32 version = VK_MAKE_VERSION(1, 0, 0));
		VkApplicationInfo info = {};

		static constexpr u32 engine_version = VK_MAKE_VERSION(1, 0, 0);
	};

	struct ValidationLayers
	{
		ValidationLayers();
		ValidationLayers(const std::vector<const char*>& layers);
		ValidationLayers(std::vector<const char*>&& layers);

		ResultValue<bool> supported() const;

		std::vector<const char*> layers;
	};

	struct Instance
	{
		Instance() = default;
		Instance(const Instance&) = delete;
		Instance(Instance&& rhs) noexcept;
		~Instance();

		Instance& operator= (const Instance&) = delete;
		Instance& operator= (Instance&& rhs) noexcept;

		static Result Create(Instance& instance, const ApplicationInfo& app = {}, const ValidationLayers& layers = {});

		void Release();

		VkInstance instance = VK_NULL_HANDLE;
	};

	template<typename T>
	static void release(T*& object, const Instance& instance)
	{
		release(object, VK_NULL_HANDLE, instance.instance);
	}
}