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
#include "Engine/Drawable/Shape.h"
#include "Engine/Graphics/CommandPool.h"
#include "Engine/Graphics/StagingBuffer.h"

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

		Result CreateShape(Shape& shape, const HeapBuffer<Vertex2>& vertices, const HeapBuffer<u16>& indices);
		Result CreateShape(Shape& shape, HeapBuffer<Vertex2>&& vertices, HeapBuffer<u16>&& indices);

	private:
		void AddDrawable(DrawableData* drawable);

	private:
		Instance instance;
		rv_debug_only(DebugMessenger debug;);
		Device device;
		MemoryAllocator allocator;
		StagingBufferManager manager;

		ShaderMap shaders;
		std::vector<std::filesystem::path> shaderpaths;

		std::vector<std::unique_ptr<DrawableData>> drawables;

		friend class Engine;
		friend class Renderer;
		friend class WindowRenderer;
		friend class GraphicsHelper;
	};
}