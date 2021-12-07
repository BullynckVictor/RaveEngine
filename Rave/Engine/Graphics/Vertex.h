#pragma once
#include "Engine/Graphics/Vulkan.h"
#include "Engine/Utility/Vector.h"
#include <array>

namespace rv
{
	namespace detail
	{
		static constexpr u32 GetFormatSize(const VkFormat format)
		{
			switch (format)
			{
				case VK_FORMAT_UNDEFINED: return 0;
				case VK_FORMAT_R8_UNORM: return 1;
				case VK_FORMAT_R8_SNORM: return 1;
				case VK_FORMAT_R8_USCALED: return 1;
				case VK_FORMAT_R8_SSCALED: return 1;
				case VK_FORMAT_R8_UINT: return 1;
				case VK_FORMAT_R8_SINT: return 1;
				case VK_FORMAT_R8_SRGB: return 1;
				case VK_FORMAT_R8G8_UNORM: return 2;
				case VK_FORMAT_R8G8_SNORM: return 2;
				case VK_FORMAT_R8G8_USCALED: return 2;
				case VK_FORMAT_R8G8_SSCALED: return 2;
				case VK_FORMAT_R8G8_UINT: return 2;
				case VK_FORMAT_R8G8_SINT: return 2;
				case VK_FORMAT_R8G8_SRGB: return 2;
				case VK_FORMAT_R8G8B8_UNORM: return 3;
				case VK_FORMAT_R8G8B8_SNORM: return 3;
				case VK_FORMAT_R8G8B8_USCALED: return 3;
				case VK_FORMAT_R8G8B8_SSCALED: return 3;
				case VK_FORMAT_R8G8B8_UINT: return 3;
				case VK_FORMAT_R8G8B8_SINT: return 3;
				case VK_FORMAT_R8G8B8_SRGB: return 3;
				case VK_FORMAT_B8G8R8_UNORM: return 3;
				case VK_FORMAT_B8G8R8_SNORM: return 3;
				case VK_FORMAT_B8G8R8_USCALED: return 3;
				case VK_FORMAT_B8G8R8_SSCALED: return 3;
				case VK_FORMAT_B8G8R8_UINT: return 3;
				case VK_FORMAT_B8G8R8_SINT: return 3;
				case VK_FORMAT_B8G8R8_SRGB: return 3;
				case VK_FORMAT_R8G8B8A8_UNORM: return 4;
				case VK_FORMAT_R8G8B8A8_SNORM: return 4;
				case VK_FORMAT_R8G8B8A8_USCALED: return 4;
				case VK_FORMAT_R8G8B8A8_SSCALED: return 4;
				case VK_FORMAT_R8G8B8A8_UINT: return 4;
				case VK_FORMAT_R8G8B8A8_SINT: return 4;
				case VK_FORMAT_R8G8B8A8_SRGB: return 4;
				case VK_FORMAT_B8G8R8A8_UNORM: return 4;
				case VK_FORMAT_B8G8R8A8_SNORM: return 4;
				case VK_FORMAT_B8G8R8A8_USCALED: return 4;
				case VK_FORMAT_B8G8R8A8_SSCALED: return 4;
				case VK_FORMAT_B8G8R8A8_UINT: return 4;
				case VK_FORMAT_B8G8R8A8_SINT: return 4;
				case VK_FORMAT_B8G8R8A8_SRGB: return 4;
				case VK_FORMAT_R16_UNORM: return 2;
				case VK_FORMAT_R16_SNORM: return 2;
				case VK_FORMAT_R16_USCALED: return 2;
				case VK_FORMAT_R16_SSCALED: return 2;
				case VK_FORMAT_R16_UINT: return 2;
				case VK_FORMAT_R16_SINT: return 2;
				case VK_FORMAT_R16_SFLOAT: return 2;
				case VK_FORMAT_R16G16_UNORM: return 4;
				case VK_FORMAT_R16G16_SNORM: return 4;
				case VK_FORMAT_R16G16_USCALED: return 4;
				case VK_FORMAT_R16G16_SSCALED: return 4;
				case VK_FORMAT_R16G16_UINT: return 4;
				case VK_FORMAT_R16G16_SINT: return 4;
				case VK_FORMAT_R16G16_SFLOAT: return 4;
				case VK_FORMAT_R16G16B16_UNORM: return 6;
				case VK_FORMAT_R16G16B16_SNORM: return 6;
				case VK_FORMAT_R16G16B16_USCALED: return 6;
				case VK_FORMAT_R16G16B16_SSCALED: return 6;
				case VK_FORMAT_R16G16B16_UINT: return 6;
				case VK_FORMAT_R16G16B16_SINT: return 6;
				case VK_FORMAT_R16G16B16_SFLOAT: return 6;
				case VK_FORMAT_R16G16B16A16_UNORM: return 8;
				case VK_FORMAT_R16G16B16A16_SNORM: return 8;
				case VK_FORMAT_R16G16B16A16_USCALED: return 8;
				case VK_FORMAT_R16G16B16A16_SSCALED: return 8;
				case VK_FORMAT_R16G16B16A16_UINT: return 8;
				case VK_FORMAT_R16G16B16A16_SINT: return 8;
				case VK_FORMAT_R16G16B16A16_SFLOAT: return 8;
				case VK_FORMAT_R32_UINT: return 4;
				case VK_FORMAT_R32_SINT: return 4;
				case VK_FORMAT_R32_SFLOAT: return 4;
				case VK_FORMAT_R32G32_UINT: return 8;
				case VK_FORMAT_R32G32_SINT: return 8;
				case VK_FORMAT_R32G32_SFLOAT: return 8;
				case VK_FORMAT_R32G32B32_UINT: return 12;
				case VK_FORMAT_R32G32B32_SINT: return 12;
				case VK_FORMAT_R32G32B32_SFLOAT: return 12;
				case VK_FORMAT_R32G32B32A32_UINT: return 16;
				case VK_FORMAT_R32G32B32A32_SINT: return 16;
				case VK_FORMAT_R32G32B32A32_SFLOAT: return 16;
				case VK_FORMAT_R64_UINT: return 8;
				case VK_FORMAT_R64_SINT: return 8;
				case VK_FORMAT_R64_SFLOAT: return 8;
				case VK_FORMAT_R64G64_UINT: return 16;
				case VK_FORMAT_R64G64_SINT: return 16;
				case VK_FORMAT_R64G64_SFLOAT: return 16;
				case VK_FORMAT_R64G64B64_UINT: return 24;
				case VK_FORMAT_R64G64B64_SINT: return 24;
				case VK_FORMAT_R64G64B64_SFLOAT: return 24;
				case VK_FORMAT_R64G64B64A64_UINT: return 32;
				case VK_FORMAT_R64G64B64A64_SINT: return 32;
				case VK_FORMAT_R64G64B64A64_SFLOAT: return 32;
			}

			return 0;
		}
	}

	template<typename T, T... Args>
	concept ArgsNotEmpty = sizeof...(Args) != 0;

	template<VkFormat... Formats>
	requires ArgsNotEmpty<VkFormat, Formats...>
	struct Vertex
	{
	public:
		static constexpr size_t size() { return sizeof...(Formats); }

	private:
		template<size_t D, VkFormat F, VkFormat... Args>
		static constexpr u32 GetStride() 
		{
			u32 stride = detail::GetFormatSize(F);
			if constexpr (D < size() - 1)
				stride += GetStride<D + 1, Args...>();
			return stride;
		}
		
		static constexpr VkVertexInputBindingDescription GetBinding()
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = GetStride<0, Formats...>();
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		template<size_t D, VkFormat F, VkFormat... Args>
		static constexpr void SetAttribute(std::array<VkVertexInputAttributeDescription, size()>& attributes, u32& location, u32& offset)
		{
			attributes[D].binding = 0;
			attributes[D].location = location;
			attributes[D].format = F;
			attributes[D].offset = offset;
			offset += detail::GetFormatSize(F);
			location += detail::GetFormatSize(F) / 4;
			if constexpr (D < size() - 1)
				SetAttribute<D + 1, Args...>(attributes, location, offset);
		}

		static constexpr std::array<VkVertexInputAttributeDescription, size()> GetAttributes()
		{
			std::array<VkVertexInputAttributeDescription, size()> attributes {};
			u32 location = 0;
			u32 offset = 0;
			SetAttribute<0, Formats...>(attributes, location, offset);
			return attributes;
		}

	public:
		static constexpr VkVertexInputBindingDescription binding = GetBinding();
		static constexpr std::array<VkVertexInputAttributeDescription, size()> attributes = GetAttributes();
	};

	template<typename V>
	concept VertexConcept = requires (V vertex)
	{
		vertex.binding;
		vertex.attributes;
		vertex.attributes.size();
		vertex.attributes.data();
		vertex.attributes.begin();
		vertex.attributes.end();
	};

	struct VertexDescriptor
	{
		VertexDescriptor() = default;

		template<VertexConcept V>
		void Set()
		{
			binding = V::binding;
			attributes = V::attributes.data();
			nAttributes = V::attributes.size();
		}

		VkVertexInputBindingDescription binding;
		const VkVertexInputAttributeDescription* attributes = nullptr;
		size_t nAttributes = 0;
	};

	struct Vertex2 : public Vertex<VK_FORMAT_R32G32_SFLOAT>
	{
		Vertex2() = default;
		Vertex2(const Vector2& position) : position(position) {}
		Vertex2(const float& x, const float& y) : position(x, y) {}

		Vector2 position;
	};

	struct TexVertex2 : public Vertex<VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32_SFLOAT>
	{
		TexVertex2() = default;
		TexVertex2(const Vector2& position, const Vector2& texture) : position(position), texture(texture) {}
		TexVertex2(const float& x, const float& y) : position(x, y) {}
		TexVertex2(const float& x, const float& y, const float& u, const float& v) : position(x, y), texture(u, v) {}

		Vector2 position;
		Vector2 texture;
	};

	struct Vertex3 : public Vertex<VK_FORMAT_R32G32B32_SFLOAT>
	{
		Vertex3() = default;
		Vertex3(const Vector3& position) : position(position) {}
		Vertex3(const float& x, const float& y, const float& z) : position(x, y, z) {}

		Vector3 position;
	};

	struct TexVertex3 : public Vertex<VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT>
	{
		TexVertex3() = default;
		TexVertex3(const Vector3& position, const Vector2& texture) : position(position), texture(texture) {}
		TexVertex3(const float& x, const float& y, const float& z) : position(x, y, z) {}
		TexVertex3(const float& x, const float& y, const float& z, const float& u, const float& v) : position(x, y, z), texture(u, v) {}

		Vector3 position;
		Vector2 texture;
	};
}