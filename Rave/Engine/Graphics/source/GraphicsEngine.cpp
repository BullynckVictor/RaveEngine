#include "Engine/Graphics/GraphicsEngine.h"
#include "Engine/Utility/Error.h"

#ifdef RV_DEBUG
#	define check_debug()		rv_rif(debug.Check())
#	define check_debug_static()	rv_rif(graphics.debug.Check())
#else
#	define check_debug()
#	define check_debug_static()
#endif

rv::GraphicsEngineInfo::GraphicsEngineInfo(const ApplicationInfo& app)
	:
	app(app)
{
}


rv::Result rv::GraphicsEngine::Create(GraphicsEngine& graphics, const GraphicsEngineInfo& info)
{
	rv_result;

	rv_rif(Instance::Create(graphics.instance, info.app));
	rv_debug_only(rv_rif(DebugMessenger::Create(graphics.debug, graphics.instance, combine(RV_SEVERITY_ERROR, RV_SEVERITY_WARNING), false)));
	check_debug_static();
	rv_rif(Device::Create(graphics.device, graphics.instance));
	check_debug_static();

	return result;
}
