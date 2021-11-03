#include "Engine/Core/Engine.h"
#include "Engine/Utility/Error.h"

rv::Result rv::Engine::Create(Engine& engine)
{
	rv_result;

	rv_rif(GraphicsEngine::Create(engine.graphics));

	return result;
}