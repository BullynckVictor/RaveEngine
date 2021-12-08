#pragma once
#include "Engine/Graphics/Device.h"

namespace rv
{
	struct Fence
	{
		Fence() = default;
		Fence(const Fence&) = delete;
		Fence(Fence&& rhs) noexcept;
		~Fence();

		Fence& operator= (const Fence&) = delete;
		Fence& operator= (Fence&& rhs) noexcept;

		void Release();

		static Result Create(Fence& fence, const Device& device, bool signaled = false);

		Result Wait(bool waitAll = true, u64 timeout = std::numeric_limits<u64>::max()) const;
		static Result Wait(std::vector<std::reference_wrapper<const Fence>> fences, bool waitAll = true, u64 timeout = std::numeric_limits<u64>::max());
		static Result Wait(const Device& device, std::vector<VkFence> fences, bool waitAll = true, u64 timeout = std::numeric_limits<u64>::max());

		Result Reset();
		static Result Wait(std::vector<std::reference_wrapper<const Fence>> fences);

		VkFence fence = VK_NULL_HANDLE;
		const Device* device = nullptr;
	};
}