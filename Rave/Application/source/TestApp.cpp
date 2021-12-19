#include "Application/TestApp.h"

TestApp::TestApp()
	:
	Application("Hello Rave!", 600, 370, rv::FColors::Silver, true, true)
{
	renderer.CreateShape(triangle, {
		{ -0.25f, -0.5f },
		{  0.50f,  0.5f },
		{ -0.50f,  0.5f },
		{  0.75f, -0.5f }
		}, {{
			0, 1, 2,
			0, 3, 1
	}}).expect("Unable to create Triangle");
}

void TestApp::Update(const rv::Duration dt)
{
	frames++;
	float s = timer.Peek().seconds();
	if (s >= 1.0f)
	{
		float fps = (float)frames / s;
		frames = 0;
		timer.Reset();
		rv_log(rv::str((int)fps, " fps"));
	}
}