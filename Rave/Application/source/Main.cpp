#include "Engine/Include.h"
#include <iostream>

rv::Result rave_main()
{
	rv_result;

	{
		rv::Engine engine;
		rv::WindowRenderer renderer;

		rv_rif(rv::Engine::Create(engine));
		rv_rif(engine.CreateWindowRenderer(renderer, "Hello World!", 480, 320));

		renderer.window.SetTitle(rv::str("size: (", renderer.window.Size().width, ", ", renderer.window.Size().height, ")"));

		while (renderer.window.HandleMessages())
		{
		}
	}

	return result;
}