#include "Engine/Graphics/Graphics.h"
#include "Engine/Utility/Error.h"
#include <algorithm>

#ifdef RV_DEBUG
#	define check_debug()		rv_rif(debug.Check())
#	define check_debug_static()	rv_rif(graphics.debug.Check())
#else
#	define check_debug()
#	define check_debug_static()
#endif

rv::GraphicsInfo::GraphicsInfo(const ApplicationInfo& app)
	:
	app(app)
{
}

rv::GraphicsInfo::GraphicsInfo(const std::vector<std::reference_wrapper<const Surface>>& surfaces, const ApplicationInfo& app)
	:
	surfaces(surfaces),
	app(app)
{
}

rv::GraphicsInfo::GraphicsInfo(const std::vector<std::reference_wrapper<const Window>>& windows, const ApplicationInfo& app)
	:
	windows(windows),
	app(app)
{
}

rv::Result rv::Graphics::Create(Graphics& graphics, const GraphicsInfo& info)
{
	rv_result;

	rv_rif(Instance::Create(graphics.instance, info.app));

	rv_debug_only(rv_rif(DebugMessenger::Create(graphics.debug, graphics.instance, combine(RV_SEVERITY_ERROR, RV_SEVERITY_WARNING), false)));
	check_debug_static();

	std::vector<std::reference_wrapper<const Surface>> surfaces; 
	surfaces.reserve(info.surfaces.size() + info.windows.size());
	std::vector<Surface> windowSurfaces; 
	windowSurfaces.reserve(info.windows.size());

	for (const auto surface : info.surfaces)
		surfaces.push_back(surface);

	for (size_t i = 0; i < info.windows.size(); ++i)
	{
		Surface surface;
		rv_rif(Surface::Create(surface, graphics.instance, info.windows[i]));
		windowSurfaces.push_back(std::move(surface));
		surfaces.push_back(windowSurfaces.back());
	}
	rv_rif(Device::Create(graphics.device, graphics.instance, DefaultDeviceRequirements(surfaces)));
	check_debug_static();

	return result;
}
