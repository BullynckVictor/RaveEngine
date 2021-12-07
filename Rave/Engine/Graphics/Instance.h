#pragma once
#include "Engine/Graphics/Vulkan.h"
#include "Engine/Utility/Result.h"
#include "Engine/Utility/Flags.h"

namespace rv
{
	struct ApplicationInfo
	{
		ApplicationInfo();
		ApplicationInfo(const char* name, u32 version = VK_MAKE_VERSION(1, 0, 0));
		VkApplicationInfo info = {};

		static constexpr u32 engine_version = VK_MAKE_VERSION(1, 0, 0);
	};

	enum ValidationLayerType
	{
		RV_VALIDATION_KHRONOS
	};

	struct ValidationLayers
	{
		ValidationLayers();
		ValidationLayers(Flags<ValidationLayerType> flags);
		ValidationLayers(const std::vector<const char*>& layers);
		ValidationLayers(std::vector<const char*>&& layers);

		ResultValue<bool> supported() const;
		void AddLayer(ValidationLayerType type);

		std::vector<const char*> layers;
	};

	enum ExtensionType
	{
		RV_EXTENSION_SURFACE,
		RV_EXTENSION_SURFACE_WIN32,
		RV_EXTENSION_DEBUG,
		RV_EXTENSION_SWAPCHAIN,
	};

	struct Extensions
	{
		Extensions();
		Extensions(Flags<ExtensionType> flags);
		Extensions(const std::vector<const char*>& extensions);
		Extensions(std::vector<const char*>&& extensions);

		void AddExtension(ExtensionType extension);

		std::vector<const char*> extensions;
	};

	struct Instance
	{
		Instance() = default;
		Instance(const Instance&) = delete;
		Instance(Instance&& rhs) noexcept;
		~Instance();

		Instance& operator= (const Instance&) = delete;
		Instance& operator= (Instance&& rhs) noexcept;

		static Result Create(Instance& instance, const ApplicationInfo& app = {}, const ValidationLayers& layers = {}, const Extensions& extensions = {});

		void Release();

		VkInstance instance = VK_NULL_HANDLE;
		ApplicationInfo app;
		ValidationLayers layers;
		Extensions extensions;
	};

	template<typename T>
	static void release(T*& object, const Instance& instance)
	{
		release(object, VK_NULL_HANDLE, instance.instance);
	}
}