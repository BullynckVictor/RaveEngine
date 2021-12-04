#include "Engine/Include.h"
#include <iostream>

rv::Result rave_main()
{
	rv_result;

	rv::Engine engine;
	rv::WindowRenderer renderer;
	rv::InputManager input(renderer.window);

	rv_rif(rv::Engine::Create(engine));
	rv_rif(rv::WindowRenderer::Create(renderer, engine, "Hello World", 600, 370));

	rv::Shape triangle;
	rv_rif(renderer.CreateShape(triangle));

	while (renderer.window.HandleMessages())
	{
		renderer.Render();
	}

	return result;
}