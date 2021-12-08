#pragma once
#include "Engine/Graphics/CommandBuffer.h"
#include "Engine/Graphics/VertexBuffer.h"

namespace rv
{
	struct DrawableData
	{
		virtual ~DrawableData() = default;
	};

	typedef void(*DrawableRecordFunction)(CommandBuffer&, const DrawableData&);

	struct DrawableRecorder
	{
		DrawableRecordFunction recordFunction = nullptr;
		const FullPipeline* pipeline = nullptr;
		DrawableData* data = nullptr;
		size_t commandIndex = 0;
	};

	template<typename D>
	class Drawable
	{
	public:
		using Data = D;

	protected:
		D* data = nullptr;

		friend class WindowRenderer;
		friend class Graphics;
		friend class GraphicsHelper;
	};

	template<typename D>
	concept DrawableConcept = requires(D drawable, CommandBuffer command, const DrawableData data, PipelineLayoutDescriptor layout, size_t index)
	{
		drawable.RecordCommand(command, data);
		drawable.DescribePipeline(layout, index);
		D::nPipelines;
	};
}