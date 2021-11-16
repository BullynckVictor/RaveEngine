#pragma once
#include "Engine/Graphics/Instance.h"
#include "Engine/Core/Window.h"

namespace rv
{
	struct Surface
	{
		Surface() = default;
		Surface(const Surface&) = delete;
		Surface(Surface&& rhs) noexcept;
		~Surface();

		Surface& operator= (const Surface&) = delete;
		Surface& operator= (Surface&& rhs) noexcept;

		void Release();

		static Result Create(Surface& surface, const Instance& instance, const Window& window);

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		const Instance* instance = nullptr;
	};
}