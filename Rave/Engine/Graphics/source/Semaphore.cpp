#include "Engine/Graphics/Semaphore.h"
#include "Engine/Utility/Error.h"

template<>
void rv::destroy(VkSemaphore semaphore, VkDevice device, VkInstance)
{
	vkDestroySemaphore(device, semaphore, nullptr);
}

rv::Semaphore::Semaphore(Semaphore&& rhs) noexcept
	:
	semaphore(move(rhs.semaphore)),
	device(move(rhs.device))
{
}

rv::Semaphore::~Semaphore()
{
	Release();
}

rv::Semaphore& rv::Semaphore::operator=(Semaphore&& rhs) noexcept
{
	semaphore = move(rhs.semaphore);
	device = move(rhs.device);
	return *this;
}

void rv::Semaphore::Release()
{
	if (device)
		release(semaphore, *device);
}

rv::Result rv::Semaphore::Create(Semaphore& semaphore, const Device& device)
{
	semaphore.Release();

	semaphore.device = &device;
	VkSemaphoreCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	return rv_try_vkr(vkCreateSemaphore(device.device, &createInfo, nullptr, &semaphore.semaphore));
}

rv::Result rv::Semaphore::Wait(u64 timeout)
{
	VkSemaphoreWaitInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
	info.pSemaphores = &semaphore;
	info.semaphoreCount = 1;
	return rv_try_vkr(vkWaitSemaphores(device->device, &info, timeout));
}

rv::Result rv::Semaphore::Wait(std::vector<std::reference_wrapper<const Semaphore>> semaphores, u64 timeout)
{
	rv_result;

	if (semaphores.empty())
		return success;

	VkDevice device = semaphores[0].get().device->device;
	std::vector<VkSemaphore> sems(semaphores.size());
	std::transform(semaphores.begin(), semaphores.end(), sems.begin(),
		[&device](const Semaphore& semaphore) {
			return semaphore.semaphore; 
			if (semaphore.device->device != device) 
				device = nullptr;
		}
	);
	rif_assert_info(device, "Not all Devices are the same");
	VkSemaphoreWaitInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
	info.pSemaphores = sems.data();
	info.semaphoreCount = (u32)sems.size();
	return rv_try_vkr(vkWaitSemaphores(device, &info, timeout));
}
