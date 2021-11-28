#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Graphics/Instance.h"
#include "Engine/Graphics/DebugMessenger.h"
#include "Engine/Graphics/Device.h"
#include "Engine/Core/Window.h"
#include "Engine/Graphics/SwapChain.h"
#include "Engine/Graphics/Shader.h"
#include "Engine/Utility/File.h"

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

		Result GetShader(Shader*& shader, const char* name, Flags<ShaderType, u32> type = {});
		Result CreateShader(const char* name, Flags<ShaderType, u32> type = {});
		Result AddShaderPath(const char* path);

	private:
		Instance instance;
		rv_debug_only(DebugMessenger debug;);
		Device device;

		std::map<Identifier, Shader> shaders;
		std::vector<std::filesystem::path> shaderpaths;

		friend class Engine;
	};
}