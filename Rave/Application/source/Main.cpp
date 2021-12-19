#include "Application/TestApp.h"

rv::Result rave_main()
{
	TestApp app;
	app.Run();
	return rv::success;
}