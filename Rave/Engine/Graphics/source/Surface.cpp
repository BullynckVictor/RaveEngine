#include "Engine/Graphics/Surface.h"
#include "Engine/Utility/Error.h"

template<>
void rv::destroy(VkSurfaceKHR surface, VkDevice device, VkInstance instance)
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
}

rv::Surface::Surface(Surface&& rhs) noexcept
	:
	surface(move(rhs.surface)),
	instance(move(rhs.instance))
{
}

rv::Surface::~Surface()
{
	Release();
}

rv::Surface& rv::Surface::operator=(Surface&& rhs) noexcept
{
	surface = move(rhs.surface);
	instance = move(rhs.instance);
	return *this;
}

void rv::Surface::Release()
{
	if (instance)
		release(surface, *instance);
	instance = nullptr;
}

rv::Result rv::Surface::Create(Surface& surface, const Instance& instance, const Window& window)
{
	surface.Release();
	surface.instance = &instance;

#	ifdef RV_PLATFORM_WINDOWS

	win32::ExposedWindow exposed = window.Expose();

	VkWin32SurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = exposed.hwnd;
	createInfo.hinstance = exposed.instance;

	return rv_try_vkr(vkCreateWin32SurfaceKHR(instance.instance, &createInfo, nullptr, &surface.surface));

#	endif
}
