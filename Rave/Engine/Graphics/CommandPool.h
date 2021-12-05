#pragma once
#include "Engine/Graphics/Device.h"

namespace rv
{
	struct CommandPool
	{
		CommandPool() = default;
		CommandPool(const CommandPool&) = delete;
		CommandPool(CommandPool&& rhs) noexcept;
		~CommandPool();

		CommandPool& operator= (const CommandPool&) = delete;
		CommandPool& operator= (CommandPool&& rhs) noexcept;

		void Release();

		static Result Create(CommandPool& pool, const Device& device, u32 family, bool reset = false);
		static Result CreateGraphics(CommandPool& pool, const Device& device, bool reset = false);

		VkCommandPool pool = VK_NULL_HANDLE;
		const Device* device = nullptr;
	};
}