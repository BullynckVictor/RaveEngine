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
#include "Engine/Utility/Multimap.h"
#include "Engine/Graphics/DescriptorSet.h"
#include <set>

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

		template<DrawableStaticData D>	
		D::StaticData& GetStaticData() { return drawableData.get<D::StaticData>(); }
		template<DrawableData D>	
		D::Data& GetData(D& drawable) { return drawableData.get<D::Data>(drawable.id()); }
		template<DrawableData D>
		D::Data& GetDataInterpreted(Drawable drawable) { return drawableData.get<D::Data>(drawable.id()); }

		Result CreateShape(Shape& shape, const HeapBuffer<Vertex2>& vertices, const HeapBuffer<u16>& indices, const FColor& color);
		Result CreateShape(Shape& shape, HeapBuffer<Vertex2>&& vertices, HeapBuffer<u16>&& indices, const FColor& color);

	private:
		template<typename D>
		bool InitStatic();

		Drawable NewDrawable();

	private:

		Instance instance;
		rv_debug_only(DebugMessenger debug;);
		Device device;
		MemoryAllocator allocator;
		StagingBufferManager manager;

		ShaderMap shaders;
		std::vector<std::filesystem::path> shaderpaths;

		MultiMap drawableData;

		uint lastDrawable = 0;
		std::list<Drawable> freeDrawables;

		DescriptorSetAllocator setAllocator = device;

		std::set<size_t> staticInitializedDrawables;

		friend class Engine;
		friend class Renderer;
		friend class WindowRenderer;
		friend class GraphicsHelper;
	};
}