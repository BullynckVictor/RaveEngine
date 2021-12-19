#include "Engine/Core/Application.h"

rv::Application::Application()
	:
	input(renderer.window)
{
	Engine::Create(engine).expect("Unable to initialize Engine");
	WindowRenderer::Create(renderer, engine, "Rave Application", 600, 370).expect("Unable to initialize Window");
}

rv::Application::Application(const std::string& title, uint width, uint height, const FColor& background, bool vsync, bool resize)
	:
	input(renderer.window)
{
	Engine::Create(engine).expect("Unable to initialize Engine");
	WindowRenderer::Create(renderer, engine, title, width, height, background, vsync, resize).expect("Unable to initialize Window");
}

rv::Application::Application(const std::string& title, const FColor& background, const WindowDescriptor& window, const SwapChainPreferences& preferences)
	:
	input(renderer.window)
{
	Engine::Create(engine).expect("Unable to initialize Engine");
	WindowRenderer::Create(renderer, engine, background, window, preferences).expect("Unable to initialize Window");
}

void rv::Application::Run()
{
	Timer timer;
	while (renderer.window.HandleMessages())
	{
		input.HandleInput();
		Update(timer.Mark());
		if (renderer.window.Closed())
			break;
		renderer.Render().expect("Render Failed");
	}
	renderer.Wait().expect();
}
