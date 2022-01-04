#pragma once
#include "Engine/Include.h"

class TestApp : public rv::Application
{
public:
	TestApp();

private:
	void Update(const rv::Duration dt) override;

private:
	struct Triangle
	{
		rv::Shape shape;
		rv::Shape::Data* data = nullptr;
	} triangle;
	rv::Timer frameTimer;
	rv::uint frames = 0;
	rv::Timer timer;
};