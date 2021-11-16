#pragma once

namespace rv
{
	class Renderer
	{
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		virtual void BeginFrame();
		virtual void EndFrame();

	protected:
	};
}