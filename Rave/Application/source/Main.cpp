#include "Engine/Include.h"
#include <iostream>

rv::Result rave_main()
{
	rv_result;

	{
		rv::Engine engine;
		rv::Window window;

		rv_rif(rv::Engine::Create(engine, "Test Application"));
		rv_rif(rv::Window::Create(window, "Test Window", 480, 320));

		rv_rif(window.SetTitle(rv::str("size: (", window.Size().width, ", ", window.Size().height, ")")));		
		while (window.HandleMessages())
		{
		}
	}

	return result;
}