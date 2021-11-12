#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Graphics/Instance.h"
#include "Engine/Graphics/DebugMessenger.h"

namespace rv
{
	struct GraphicsEngineInfo
	{
		GraphicsEngineInfo(const ApplicationInfo& app = {});
		ApplicationInfo app;
	};

	class GraphicsEngine
	{
	public:
		GraphicsEngine() = default;
		static Result Create(GraphicsEngine& graphics, const GraphicsEngineInfo& info = {});

		Result CheckResults();

	private:
		Instance instance;
		rv_debug_only(DebugMessenger debug;);
	};
}