#pragma once
#include "Engine/Graphics/Device.h"

namespace rv
{
	namespace detail
	{
		static constexpr u32 get_flag_bit(u32 flag)
		{
			for (u32 i = 0; ; ++i)
				if ((flag /= 2) == 0)
					return i;
		}
	}

	enum ShaderType
	{
		RV_ST_VERTEX 					= detail::get_flag_bit(VK_SHADER_STAGE_VERTEX_BIT 					),
		RV_ST_TESSELLATION_CONTROL 		= detail::get_flag_bit(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT 	),
		RV_ST_TESSELLATION_EVALUATION 	= detail::get_flag_bit(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT 	),
		RV_ST_GEOMETRY 					= detail::get_flag_bit(VK_SHADER_STAGE_GEOMETRY_BIT 				),
		RV_ST_FRAGMENT 					= detail::get_flag_bit(VK_SHADER_STAGE_FRAGMENT_BIT 				),
		RV_ST_COMPUTE 					= detail::get_flag_bit(VK_SHADER_STAGE_COMPUTE_BIT 					),
		RV_ST_RAYGEN 					= detail::get_flag_bit(VK_SHADER_STAGE_RAYGEN_BIT_KHR 				),
		RV_ST_ANY_HIT 					= detail::get_flag_bit(VK_SHADER_STAGE_ANY_HIT_BIT_KHR 				),
		RV_ST_CLOSEST_HIT 				= detail::get_flag_bit(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR 			),
		RV_ST_MISS 						= detail::get_flag_bit(VK_SHADER_STAGE_MISS_BIT_KHR 				),
		RV_ST_INTERSECTION 				= detail::get_flag_bit(VK_SHADER_STAGE_INTERSECTION_BIT_KHR 		),
		RV_ST_CALLABLE 					= detail::get_flag_bit(VK_SHADER_STAGE_CALLABLE_BIT_KHR 			),
		RV_ST_TASK		 				= detail::get_flag_bit(VK_SHADER_STAGE_TASK_BIT_NV		 		 	),
		RV_ST_MESH 						= detail::get_flag_bit(VK_SHADER_STAGE_MESH_BIT_NV 					),
	};

	struct Shader
	{
		Shader() = default;
		Shader(const Shader&) = delete;
		Shader(Shader&& rhs) noexcept;
		~Shader();

		Shader& operator= (const Shader&) = delete;
		Shader& operator= (Shader&& rhs) noexcept;

		void Release();

		static Result Create(Shader& shader, const Device& device, const char* filename, Flags<ShaderType, u32> type = {});
		static Result Compile(const char* source, const char* output);
		static Result CompileAndCreate(Shader& shader, const Device& device, const char* source, const char* output, Flags<ShaderType, u32> type = {});

		static Flags<ShaderType, u32> GetShaderTypeFromFile(const char* path);

		VkShaderModule shader = VK_NULL_HANDLE;
		Flags<ShaderType, u32> type;
		const Device* device = nullptr;

		static const char* vulkan_path;
	};
}