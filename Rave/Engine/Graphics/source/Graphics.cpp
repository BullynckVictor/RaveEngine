#include "Engine/Graphics/Graphics.h"
#include "Engine/Graphics/GraphicsHelper.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"
#include "Engine/Utility/File.h"
#include "Engine/Core/Logger.h"
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

	rv_rif(MemoryAllocator::Create(graphics.allocator, graphics.instance, graphics.device));
	check_debug_static();

	rv_rif(StagingBufferManager::Create(graphics.manager, graphics.device, graphics.allocator, graphics.device.computeQueue));
	check_debug_static();

	return result;
}

rv::Result rv::Graphics::GetShader(Shader*& outshader, const char* name, ShaderType type)
{
	outshader = nullptr;
	auto shader = shaders.find(name);
	if (shader == shaders.end())
	{
		std::filesystem::path path(name);
		path += ".spv";
		if (!FileExists(path))
		{
			bool found = false;
			for (const auto& d : shaderpaths)
			{
				auto p = std::filesystem::absolute(d);
				if (FileExists(p += path))
				{
					found = true;
					path = p;
					break;
				}
				else if (FileExists((p = std::filesystem::absolute(d)) += std::filesystem::path("/bin/") += path))
				{
					found = true;
					path = p;
					break;
				}
			}
			if (!found)
				return rv_runtime_error(str("Shader \"", name, "\" not found"));
		}
		Shader& s = shaders[name];
		Result result = Shader::Create(s, device, path.string().c_str(), type);
		if (result.succeeded())
			outshader = &s;
		return result;
	}
	outshader = &shader->second;
	return success;
}

rv::Result rv::Graphics::CreateShader(const char* name, ShaderType type)
{
	Shader* s = nullptr;
	return GetShader(s, name, type);
}

rv::Result rv::Graphics::AddShaderPath(const char* path)
{
	if (std::filesystem::is_directory(path))
		shaderpaths.push_back(path);
	else
		return failure;
	return success;
}

rv::Result rv::Graphics::CreateShape(Shape& shape, const HeapBuffer<Vertex2>& vertices, const HeapBuffer<u16>& indices)
{
	GraphicsHelper::Manage(*this, shape);
	return Shape::Create(shape, manager, vertices, indices);
}

rv::Result rv::Graphics::CreateShape(Shape& shape, HeapBuffer<Vertex2>&& vertices, HeapBuffer<u16>&& indices)
{
	GraphicsHelper::Manage(*this, shape);
	return Shape::Create(shape, manager, std::move(vertices), indices);
}

void rv::Graphics::AddDrawable(DrawableData* drawable)
{
	drawables.emplace_back(drawable);
}