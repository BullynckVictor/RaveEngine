#include "Engine/Core/Engine.h"
#include "Engine/Core/Logger.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/Timer.h"
#include "Engine/Utility/String.h"

rv::Result rv::Engine::Create(Engine& engine, const GraphicsEngineInfo& info)
{
	rv_result;
	rv_debug_logger_only(Timer timer);

	rv_rif(GraphicsEngine::Create(engine.graphics, info));

	rv_log(str("Created Engine in ", timer.Mark().seconds(), "s"));
	return result;
}

rv::Result rv::Engine::Create(Engine& engine, const char* appName, const u32 version)
{
	GraphicsEngineInfo info = ApplicationInfo(appName, version);
	return Create(engine, info);
}
