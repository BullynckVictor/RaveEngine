#include "Application/TestApp.h"

TestApp::TestApp()
	:
	Application("Hello Rave!", 600, 370, rv::FColors::Silver, true, true)
{
	renderer.CreateShape(triangle.shape, {
		{ -0.25f, -0.5f },
		{  0.50f,  0.5f },
		{ -0.50f,  0.5f },
		{  0.75f, -0.5f }
		}, {{
			0, 1, 2,
			0, 3, 1
	}}, rv::FColors::Maroon).expect("Unable to create Triangle");
	triangle.data = &engine.graphics.GetData(triangle.shape);
	triangle.data->color;
	frameTimer.Reset();
}

void TestApp::Update(const rv::Duration dt)
{
	frames++;
	float s = frameTimer.Peek().seconds();
	float y = (sinf(timer.Peek().seconds()) + 1.0f) / 2.0f;
	triangle.data->color.r = y;

	if (s >= 1.0f)
	{
		float fps = (float)frames / s;
		frames = 0;
		frameTimer.Reset();
		rv_log(rv::str((int)fps, " fps"));
	}
}