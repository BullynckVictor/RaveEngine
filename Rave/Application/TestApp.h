#pragma once
#include "Engine/Include.h"

class TestApp : public rv::Application
{
public:
	TestApp();

private:
	void Update(const rv::Duration dt) override;

private:
	rv::Shape triangle;
	rv::Timer timer;
	rv::uint frames = 0;
};