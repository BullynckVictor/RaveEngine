#pragma once
#include "Engine/Core/CompileTimeInfo.h"
#ifdef RV_PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#include "Engine/Core/SystemInclude.h"
#endif
#include <vulkan/vulkan.h>

namespace rv
{
	template<typename T>
	void destroy(T* object, VkDevice device = VK_NULL_HANDLE, VkInstance instance = VK_NULL_HANDLE);

	template<typename T>
	static T* move(T*& dead)
	{
		T* temp = dead;
		dead = nullptr;
		return temp;
	}


	template<typename T>
	static void release(T*& object, VkDevice device = VK_NULL_HANDLE, VkInstance instance = VK_NULL_HANDLE)
	{
		if (object)
		{
			destroy(object, device, instance);
			object = nullptr;
		}
	}
}