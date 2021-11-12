#include "Engine/Graphics/GraphicsEngine.h"
#include "Engine/Utility/Error.h"

rv::GraphicsEngineInfo::GraphicsEngineInfo(const ApplicationInfo& app)
	:
	app(app)
{
}


rv::Result rv::GraphicsEngine::Create(GraphicsEngine& graphics, const GraphicsEngineInfo& info)
{
	rv_result;

	rv_rif(Instance::Create(graphics.instance, info.app));

	rv_debug_only(rv_rif(DebugMessenger::Create(graphics.debug, graphics.instance)));

	return result;
}

rv::Result rv::GraphicsEngine::CheckResults()
{
	rv_debug_only(return debug.Check());
	return success;
}
