#include "Engine/Graphics/Fence.h"
#include "Engine/Utility/Error.h"

template<>
void rv::destroy(VkFence fence, VkDevice device, VkInstance)
{
	vkDestroyFence(device, fence, nullptr);
}

rv::Fence::Fence(Fence&& rhs) noexcept
	:
	fence(move(rhs.fence)),
	device(move(rhs.device))
{
}

rv::Fence::~Fence()
{
	Release();
}

rv::Fence& rv::Fence::operator=(Fence&& rhs) noexcept
{
	fence = move(rhs.fence);
	device = move(rhs.device);
	return *this;
}

void rv::Fence::Release()
{
	if (fence)
		Wait();
	if (device)
		release(fence, *device);
}

rv::Result rv::Fence::Create(Fence& fence, const Device& device, bool signaled)
{
	fence.Release();

	fence.device = &device;
	VkFenceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	if (signaled)
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	return rv_try_vkr(vkCreateFence(device.device, &createInfo, nullptr, &fence.fence));
}

rv::Result rv::Fence::Wait(bool waitAll, u64 timeout) const
{
	return rv_try_vkr(vkWaitForFences(device->device, 1, &fence, waitAll, timeout));
}

rv::Result rv::Fence::Wait(std::vector<std::reference_wrapper<const Fence>> fences, bool waitAll, u64 timeout)
{
	rv_result;

	if (fences.empty())
		return success;

	VkDevice device = fences[0].get().device->device;
	std::vector<VkFence> fens(fences.size());
	std::transform(fences.begin(), fences.end(), fens.begin(),
		[&device](const Fence& fence) {
			return fence.fence;
			if (fence.device->device != device)
				device = nullptr;
		}
	);
	rif_assert_info(device, "Not all Devices are the same");
	return rv_try_vkr(vkWaitForFences(device, (u32)fens.size(), fens.data(), waitAll, timeout));
}

rv::Result rv::Fence::Wait(const Device& device, std::vector<VkFence> fences, bool waitAll, u64 timeout)
{
	if (fences.empty())
		return success;

	return rv_try_vkr(vkWaitForFences(device.device, (u32)fences.size(), fences.data(), waitAll, timeout));
}

rv::Result rv::Fence::Reset()
{
	return rv_try_vkr(vkResetFences(device->device, 1, &fence));
}

rv::Result rv::Fence::Wait(std::vector<std::reference_wrapper<const Fence>> fences)
{
	rv_result;

	if (fences.empty())
		return success;

	VkDevice device = fences[0].get().device->device;
	std::vector<VkFence> fens(fences.size());
	std::transform(fences.begin(), fences.end(), fens.begin(),
		[&device](const Fence& fence) {
			return fence.fence;
			if (fence.device->device != device)
				device = nullptr;
		}
	);
	rif_assert_info(device, "Not all Devices are the same");
	return rv_try_vkr(vkResetFences(device, (u32)fens.size(), fens.data()));
}
