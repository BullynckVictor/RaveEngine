#include "Engine/Core/Engine.h"
#include "Engine/Utility/Error.h"

rv::Result rv::Engine::Create(Engine& engine, const GraphicsEngineInfo& info)
{
	rv_result;

	rv_rif(GraphicsEngine::Create(engine.graphics, info));

	return result;
}

rv::Result rv::Engine::Create(Engine& engine, const char* appName, const u32 version)
{
	GraphicsEngineInfo info = ApplicationInfo(appName, version);
	return Create(engine, info);
}
