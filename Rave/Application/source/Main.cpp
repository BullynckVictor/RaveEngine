#include "Engine/Include.h"
#include <iostream>

rv::Result rave_main()
{
	rv_result;

	rv::Engine engine;
	rv::WindowRenderer renderer;
	rv::InputManager input(renderer.window);

	rv::Shader* fragment;
	rv::Shader* vertex;

	rv_rif(rv::Engine::Create(engine));
	rv_rif(engine.CreateWindowRenderer(renderer, "Hello world", 600, 370));

	rv_rif(engine.graphics.GetShader(vertex,	"triangle.vert"));
	rv_rif(engine.graphics.GetShader(fragment,	"triangle.frag"));

	while (renderer.window.HandleMessages())
	{

	}

	return result;
}