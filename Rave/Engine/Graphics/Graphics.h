#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Graphics/Instance.h"
#include "Engine/Graphics/DebugMessenger.h"
#include "Engine/Graphics/Device.h"
#include "Engine/Core/Window.h"
#include "Engine/Graphics/SwapChain.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Utility/File.h"
#include "Engine/Graphics/Pipeline.h"
#include "Engine/Drawable/Drawable.h"
#include "Engine/Graphics/MemoryAllocator.h"

namespace rv
{
	struct GraphicsInfo
	{
		GraphicsInfo() = default;
		GraphicsInfo(const ApplicationInfo& app);
		GraphicsInfo(const std::vector<std::reference_wrapper<const Surface>>& surfaces, const ApplicationInfo& app = {});
		GraphicsInfo(const std::vector<std::reference_wrapper<const Window>>& windows, const ApplicationInfo& app = {});

		ApplicationInfo app;
		std::vector<std::reference_wrapper<const Surface>> surfaces;
		std::vector<std::reference_wrapper<const Window>> windows;
	};

	class Graphics
	{
	public:
		Graphics() = default;
		static Result Create(Graphics& graphics, const GraphicsInfo& info = {});

		Result GetShader(Shader*& shader, const char* name, ShaderType type = RV_ST_NULL);
		Result CreateShader(const char* name, ShaderType type = RV_ST_NULL);
		Result AddShaderPath(const char* path);

	private:
		void AddDrawable(DrawableData* drawable);

	private:
		Instance instance;
		rv_debug_only(DebugMessenger debug;);
		Device device;
		MemoryAllocator allocator;

		ShaderMap shaders;
		std::vector<std::filesystem::path> shaderpaths;

		std::vector<std::unique_ptr<DrawableData>> drawables;

		friend class Engine;
		friend class Renderer;
		friend class WindowRenderer;
		friend class WindowRendererHelper;
	};
}