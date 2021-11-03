#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Graphics/GraphicsEngine.h"

namespace rv
{
	class Engine
	{
	public:
		static Result Create(Engine& engine);

		GraphicsEngine graphics;
	};
}