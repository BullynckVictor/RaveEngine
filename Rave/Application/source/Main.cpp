#include "Engine/Include.h"
#include <iostream>

rv::Result rave_main()
{
	rv_result;

	{
		rv::Engine engine;
		rv_rif(rv::Engine::Create(engine, "Test Application"));
	}

	return result;
}