#include "Engine/Core/Engine.h"
#include "Engine/Core/Logger.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/Timer.h"
#include "Engine/Utility/String.h"

#ifdef RV_DEBUG
#	define check_debug()		rv_rif(graphics.debug.Check())
#	define check_debug_static()	rv_rif(engine.graphics.debug.Check())
#else
#	define check_debug()
#	define check_debug_static()
#endif

rv::Result rv::Engine::Create(Engine& engine, const GraphicsInfo& info)
{
	rv_result;
	rv_debug_logger_only(Timer timer);

	rv_rif(Graphics::Create(engine.graphics, info));
	rv_rif(engine.graphics.AddShaderPath("../Engine/Graphics/Shaders/"));

	rv_log(str("Created Engine in ", timer.Mark().seconds(), "s"));
	return result;
}

rv::Result rv::Engine::Create(Engine& engine, const char* appName, const u32 version)
{
	GraphicsInfo info = ApplicationInfo(appName, version);
	return Create(engine, info);
}

rv::Result rv::Engine::Create(Engine& engine, const std::vector<std::reference_wrapper<const Surface>>& surfaces)
{
	GraphicsInfo info = surfaces;
	return Create(engine, info);
}

rv::Result rv::Engine::Create(Engine& engine, const std::vector<std::reference_wrapper<const Window>>& windows)
{
	GraphicsInfo info = windows;
	return Create(engine, info);
}

rv::Result rv::Engine::Create(Engine& engine, const Surface& surface)
{
	GraphicsInfo info = GraphicsInfo({ surface });
	return Create(engine, info);
}

rv::Result rv::Engine::Create(Engine& engine, const Window& window)
{
	GraphicsInfo info = GraphicsInfo({ window });
	return Create(engine, info);
}