#include "Engine/Include.h"
#include <iostream>

rv::Result rave_main()
{
	rv_result;

	rv::Engine engine;

	rv_rif(rv::Engine::Create(engine));

	uint32_t extensionCount = 0;
	rif_try_vkr(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));

	rif_try_vkr(VK_ERROR_DEVICE_LOST);

	std::cout << extensionCount << " extensions supported\n";
	
	std::cin.ignore();
	return result;
}