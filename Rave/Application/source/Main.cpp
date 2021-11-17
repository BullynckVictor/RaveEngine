#include "Engine/Include.h"
#include <iostream>
#include <thread>

rv::Window* w = nullptr;
bool active = true;
rv::EventListener* plistener = nullptr;

void print_info()
{
	while (w && plistener && active)
	{
		while (rv::Event e = plistener->GetEvent())
		{
			if (e.IsType<rv::WindowClosedEvent>())
				rv_log("Window closed");
			else if (e.IsType<rv::WindowMovedEvent>())
				rv_log(rv::str("Window moved: (", e.get<rv::WindowMovedEvent>().position.x, ", ", e.get<rv::WindowMovedEvent>().position.x, ")"));
		}
	}
}

rv::Result rave_main()
{
	rv_result;

	{
		rv::EventListener listener;
		rv::Engine engine;
		rv::WindowRenderer renderer;

		rv_rif(rv::Engine::Create(engine));
		rv_rif(engine.CreateWindowRenderer(renderer, "Hello World!", 480, 320));

		w = &renderer.window;

		listener.Listen(renderer.window);
		renderer.window.SetTitle(rv::str("size: (", renderer.window.Size().width, ", ", renderer.window.Size().height, ")"));

		plistener = &listener;
		std::thread thread(print_info);

		while (renderer.window.HandleMessages())
		{
		}

		active = false;
		thread.join();
	}

	return result;
}