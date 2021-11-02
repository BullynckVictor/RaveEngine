#include <iostream>
#include "Engine/Core/Main.h"

int main()
{
	rv::Result result = rave_main();
	std::cin.ignore();
	return result.fatal() ? EXIT_FAILURE : EXIT_SUCCESS;
}