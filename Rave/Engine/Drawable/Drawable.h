#pragma once
#include "Engine/Graphics/CommandBuffer.h"
#include "Engine/Graphics/StagingBuffer.h"
#include "Engine/Graphics/DescriptorSet.h"
#include "Engine/Utility/HeapBuffer.h"
#include "Engine/Utility/Optional.h"
#include "Engine/Utility/PrimitiveType.h"

namespace rv
{
	class Drawable
	{
	public:
		constexpr Drawable() = default;
		constexpr Drawable(uint id) : m_id(id) {}

		constexpr uint id() const { return m_id.value; }

		constexpr void set(Drawable newID) { m_id = newID.m_id; }

		constexpr bool valid() const { return m_id.valid(); }
		constexpr bool invalid() const { return m_id.invalid(); }

	private:
		Optional<uint> m_id;
	};

	class Graphics;
	class Renderer;
	struct DrawableRecorder;

	typedef void (*DrawableRecordFunction)(CommandBuffer&, Graphics&, Renderer&, const DrawableRecorder&, u32);

	struct DrawableRecorder
	{
		DrawableRecorder() = default;

		DrawableRecordFunction recordFunction = nullptr;
		Drawable drawable;
		FullPipeline* pipeline = nullptr;
		u32 commandIndex = 0;
	};

	class Engine;
	class Graphics;
	class Renderer;

	enum DrawableTypeMeta
	{
		RV_DRAWABLE_STATIC_DATA,
		RV_DRAWABLE_DATA,
		RV_DRAWABLE_RENDERER_DATA,
		RV_DRAWABLE_IMAGE_DATA,
		RV_DRAWABLE_UNIQUE_PIPELINE
	};

	template<typename Parent, DrawableTypeMeta... M>
	class DrawableType : public Drawable
	{
	public:
		static constexpr struct Info
		{
			static constexpr Flags<DrawableTypeMeta> meta = combine<DrawableTypeMeta>(M...);
			static constexpr bool has_static_data() { return meta.contain(RV_DRAWABLE_STATIC_DATA); }
			static constexpr bool has_data() { return meta.contain(RV_DRAWABLE_DATA); }
			static constexpr bool has_renderer_data() { return meta.contain(RV_DRAWABLE_RENDERER_DATA); }
			static constexpr bool has_image_data() { return meta.contain(RV_DRAWABLE_IMAGE_DATA); }
			static constexpr bool has_unique_pipeline() { return  meta.contain(RV_DRAWABLE_UNIQUE_PIPELINE); }
			static constexpr bool has_static_pipeline() { return !meta.contain(RV_DRAWABLE_UNIQUE_PIPELINE); }
		}
		info;
	};

	template<typename D>
	concept DrawableConcept = std::is_base_of_v<Drawable, D> && requires(D drawable, Graphics graphics, PipelineLayoutDescriptor layout, u32 index)
	{
		drawable.info;
		drawable.info.meta;
		drawable.info.has_static_data();
		drawable.info.has_data();
		drawable.info.has_renderer_data();
		drawable.info.has_image_data();
		drawable.info.has_unique_pipeline();
		drawable.info.has_static_pipeline();
		drawable.id();
		drawable.valid();
		drawable.invalid();
		drawable.nPipelines;
		drawable.DescribePipeline(graphics, layout, index);
	};

	template<typename D> concept DrawableStaticData		= DrawableConcept<D> && D::Info::has_static_data();
	template<typename D> concept DrawableData			= DrawableConcept<D> && D::Info::has_data();
	template<typename D> concept DrawableRendererData	= DrawableConcept<D> && D::Info::has_renderer_data();
	template<typename D> concept DrawableImageData		= DrawableConcept<D> && D::Info::has_image_data();
	template<typename D> concept DrawableStaticPipeline	= DrawableConcept<D> && D::Info::has_static_pipeline();
	template<typename D> concept DrawableUniquePipeline	= DrawableConcept<D> && D::Info::has_unique_pipeline();
}