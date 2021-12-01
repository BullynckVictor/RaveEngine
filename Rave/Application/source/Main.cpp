#include "Engine/Include.h"
#include <iostream>

rv::Result rave_main()
{
	rv_result;

	rv::Engine engine;
	rv::WindowRenderer renderer;
	rv::InputManager input(renderer.window);

	rv_rif(rv::Engine::Create(engine));
	rv_rif(engine.CreateWindowRenderer(renderer, "Hello world", 600, 370));

	rv::FullPipeline* pipeline;
	rv::PipelineLayoutDescriptor layout;
	layout.shaders = {
		"triangle.vert",
		"triangle.frag"
	};
	layout.renderpass = "color";
	layout.rehash();
	rv_rif(renderer.GetPipeline(pipeline, layout));

	while (renderer.window.HandleMessages())
	{

	}

	return result;
}