#pragma once
#include "Engine/Graphics/CommandBuffer.h"

namespace rv
{
	struct DrawableData
	{
		virtual ~DrawableData() = default;

		virtual void RecordCommand(CommandBuffer& command, u32 index) const {}
	};

	template<typename T>
	concept DrawableDataConcept = std::is_base_of_v<DrawableData, T> && requires(T drawable, CommandBuffer cmd, u32 i)
	{
		drawable.RecordCommand(cmd, i);
		drawable.Initialised();
		drawable.pipeline;
	};

	template<DrawableDataConcept D>
	class Drawable
	{
	public:
		using Data = D;

	protected:
		D* data = nullptr;

		friend class WindowRendererHelper;
	};
}