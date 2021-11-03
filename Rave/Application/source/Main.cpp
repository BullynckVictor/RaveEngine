#include "Engine/Include.h"
#include <iostream>

rv::Result rave_main()
{
	rv_result;

	rv::Engine engine;

	rv::debug.Log("Hello!");

	rv_rif(rv::Engine::Create(engine, "Test Application"));

	rv::debug.Log("Hi!");
	std::cin.ignore();
	return result;
}