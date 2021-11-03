#pragma once
#include "Engine/Core/CompileTimeInfo.h"
#ifdef RV_PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.h>

namespace rv
{
	template<typename T>
	void destroy(T* object, VkDevice* device = nullptr, VkInstance* instance = nullptr);

	template<typename T>
	static T* move(T*& dead)
	{
		T* temp = dead;
		dead = nullptr;
		return temp;
	}


	template<typename T>
	static void release(T*& object)
	{
		if (object)
		{
			destroy(object);
			object = nullptr;
		}
	}
}