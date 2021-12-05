#include "Engine/Graphics/CommandPool.h"
#include "Engine/Utility/Error.h"

template<>
void rv::destroy(VkCommandPool pool, VkDevice device, VkInstance)
{
	vkDestroyCommandPool(device, pool, nullptr);
}

rv::CommandPool::CommandPool(CommandPool&& rhs) noexcept
	:
	pool(move(rhs.pool)),
	device(move(rhs.device))
{
}

rv::CommandPool::~CommandPool()
{
	Release();
}

rv::CommandPool& rv::CommandPool::operator=(CommandPool&& rhs) noexcept
{
	pool = move(rhs.pool);
	device = move(rhs.device);
	return *this;
}

void rv::CommandPool::Release()
{
	if (device)
		release(pool, *device);
}

rv::Result rv::CommandPool::Create(CommandPool& pool, const Device& device, u32 family, bool reset)
{
	pool.Release();

	pool.device = &device;
	VkCommandPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = family;
	createInfo.flags = reset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;
	return rv_try_vkr(vkCreateCommandPool(device.device, &createInfo, nullptr, &pool.pool));
}

rv::Result rv::CommandPool::CreateGraphics(CommandPool& pool, const Device& device, bool reset)
{
	return Create(pool, device, device.graphicsQueue.family, reset);
}
