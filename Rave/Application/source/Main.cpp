#include "Engine/Include.h"
#include <iostream>

rv::Result rave_main()
{
	rv_result;

	rv::Engine engine;
	rv::WindowRenderer renderer;
	rv::InputManager input(renderer.window);

	rv_rif(rv::Engine::Create(engine));
	rv_rif(rv::WindowRenderer::Create(renderer, engine, "Hello World", 600, 370, rv::FColors::Silver, true, true));

	rv::Shape triangle;
	rv_rif(renderer.CreateShape(triangle, {
			{ -0.25f, -0.5f },
			{  0.50f,  0.5f },
			{ -0.50f,  0.5f },
			{  0.75f, -0.5f }
	}, {{ 
			0, 1, 2,
			0, 3, 1
	}}));

	bool vsync = true;

	while (renderer.window.HandleMessages())
	{
		rv_rif(renderer.Render());
	}

	return renderer.Wait();
}