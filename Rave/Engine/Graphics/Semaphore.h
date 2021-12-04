#pragma once
#include "Engine/Graphics/Device.h"

namespace rv
{
	struct Semaphore
	{
		Semaphore() = default;
		Semaphore(const Semaphore&) = delete;
		Semaphore(Semaphore&& rhs) noexcept;
		~Semaphore();

		Semaphore& operator= (const Semaphore&) = delete;
		Semaphore& operator= (Semaphore&& rhs) noexcept;

		void Release();

		static Result Create(Semaphore& semaphore, const Device& device);

		Result Wait(u64 timeout = std::numeric_limits<u64>::max());
		static Result Wait(std::vector<std::reference_wrapper<const Semaphore>> semaphores, u64 timeout = std::numeric_limits<u64>::max());

		VkSemaphore semaphore = VK_NULL_HANDLE;
		const Device* device = nullptr;
	};
}