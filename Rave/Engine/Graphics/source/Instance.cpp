#include "Engine/Graphics/Instance.h"
#include "Engine/Graphics/DebugMessenger.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"
#include "Engine/Core/Logger.h"

template<>
void rv::destroy(VkInstance instance, VkDevice device, VkInstance i)
{
	vkDestroyInstance(instance, nullptr);
}

rv::ApplicationInfo::ApplicationInfo()
{
	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pApplicationName = "Rave Application";
	info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	info.pEngineName = "RaveEngine";
	info.engineVersion = engine_version;
	info.apiVersion = VK_API_VERSION_1_0;
}

rv::ApplicationInfo::ApplicationInfo(const char* name, u32 version)
{
	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pApplicationName = name;
	info.applicationVersion = version;
	info.pEngineName = "RaveEngine";
	info.engineVersion = engine_version;
	info.apiVersion = VK_API_VERSION_1_0;
}

rv::ValidationLayers::ValidationLayers()
	:
#ifdef RV_DEBUG
	layers({ "VK_LAYER_KHRONOS_validation" })
#else
	layers()
#endif
{
}

rv::ValidationLayers::ValidationLayers(const std::vector<const char*>& layers)
	:
	layers(layers)
{
}

rv::ValidationLayers::ValidationLayers(std::vector<const char*>&& layers)
	:
	layers(std::move(layers))
{
}

rv::ResultValue<bool> rv::ValidationLayers::supported() const
{
	rv_result;

	u32 layerCount = 0;
	rif_assert_vkr(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
	std::vector<VkLayerProperties> availableLayers(layerCount);
	rif_assert_vkr(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

	for (const char* layer : layers)
	{
		bool supported = false;
		for (const auto& l : availableLayers)
		{
			if (strcmp(layer, l.layerName) == 0)
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

rv::Instance::Instance(Instance&& rhs) noexcept
	:
	instance(move(rhs.instance))
{
}

rv::Instance::~Instance()
{
	Release();
}

rv::Instance& rv::Instance::operator=(Instance&& rhs) noexcept
{
	instance = move(rhs.instance);
	return *this;
}

rv::Result rv::Instance::Create(Instance& instance, const ApplicationInfo& app, const ValidationLayers& layers)
{
	rv_result;

	{
		auto supported = layers.supported();
		rv_rif(supported);
		rif_check_info(supported.value, "Not all requested validation layers supported");
	}

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &app.info;

	std::vector<const char*> extensions;
	extensions.reserve(3);
	extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	if constexpr (cti.platform.windows)
		extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	if constexpr (cti.build.debug)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	createInfo.enabledExtensionCount = (u32)extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = (u32)layers.layers.size();
	createInfo.ppEnabledLayerNames = layers.layers.data();


	if constexpr (cti.build.debug)
	{
		auto debug = DebugMessenger::CreateInfo();
		debug.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.pNext = &debug;
		rif_try_vkr_info(vkCreateInstance(&createInfo, nullptr, &instance.instance), "Unable to create Instance");
	}
	else
	{
		rif_try_vkr_info(vkCreateInstance(&createInfo, nullptr, &instance.instance), "Unable to create Instance");
	}
	rv_log(str("Created Instance \"", app.info.pApplicationName, '\"'));

	return result;
}

void rv::Instance::Release()
{
	release(instance);
}

