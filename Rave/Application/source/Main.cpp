#include "Engine/Include.h"
#include <iostream>

rv::Result rave_main()
{
	rv_result;

	rv::Engine engine;

	rv_rif(rv::Engine::Create(engine, "Test Application"));

	rv::u32 extensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	for (const auto& e : extensions)
		std::cout << e.extensionName << '\n';

	std::cin.ignore();
	return result;
}