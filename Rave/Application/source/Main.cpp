#include "Engine/Include.h"
#include <iostream>
#include <thread>

rv::Result rave_main()
{
	rv_result;

	rv::EventListener listener;
	rv::Engine engine;
	rv::WindowRenderer renderer;
	rv::InputManager input(renderer.window);

	std::string title = "Hello World!";

	rv_rif(rv::Engine::Create(engine));
	rv_rif(engine.CreateWindowRenderer(renderer, title, 480, 320));

	while (renderer.window.HandleMessages())
	{
		input.HandleInput();

		renderer.window.SetTitle(rv::str("( ", input.mouse.Position().x, ", ", input.mouse.Position().y, " )"));

		if (input.mouse.left.Flagged())
			renderer.window.Close();
	}

	return result;
}