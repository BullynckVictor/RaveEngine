#include "Engine/Graphics/Shader.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"
#include "Engine/Utility/HeapBuffer.h"
#include <filesystem>
#include <fstream>

const char* rv::Shader::vulkan_path = nullptr;

template<>
void rv::destroy(VkShaderModule shader, VkDevice device, VkInstance instance)
{
	vkDestroyShaderModule(device, shader, nullptr);
}

rv::Shader::Shader(Shader&& rhs) noexcept
	:
	shader(move(rhs.shader)),
	type(std::move(rhs.type)),
	device(move(rhs.device))
{
}

rv::Shader::~Shader()
{
	Release();
}

rv::Shader& rv::Shader::operator=(Shader&& rhs) noexcept
{
	shader = move(rhs.shader);
	type = std::move(rhs.type);
	device = move(rhs.device);
	return *this;
}

void rv::Shader::Release()
{
	if (device)
		release(shader, *device);
	type.clear();
	device = nullptr;
}

rv::Result rv::Shader::Create(Shader& shader, const Device& device, const char* filename, Flags<ShaderType, u32> type)
{
	shader.device = &device;
	rv_result;

	if (type.empty())
		type = GetShaderTypeFromFile(filename);

	rif_try_file(filename);

	size_t filesize = std::filesystem::file_size(filename);
	HeapBuffer<char> code(filesize);

	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
		return failed_file;

	file.read(code.data(), filesize);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const u32*>(code.data());
	
	return rv_try_vkr(vkCreateShaderModule(device.device, &createInfo, nullptr, &shader.shader));
}

rv::Result rv::Shader::Compile(const char* source, const char* output)
{
	if (!vulkan_path)
		vulkan_path = System::GetEnv("VULKAN_SDK");

	int result = system(str(vulkan_path, "\\Bin32\\glslc ", source, " -o ", output).c_str());
	return result == EXIT_SUCCESS ? success : failure;
}

rv::Result rv::Shader::CompileAndCreate(Shader& shader, const Device& device, const char* source, const char* output, Flags<ShaderType, u32> type)
{
	rv_result;
	rv_rif(Compile(source, output));
	return Create(shader, device, output, type);
}

rv::Flags<rv::ShaderType, rv::u32> rv::Shader::GetShaderTypeFromFile(const char* path)
{
	Flags<ShaderType, u32> type;

	if (strstr(path, ".vert"))
		type |= RV_ST_VERTEX;
	if (strstr(path, ".tesc"))
		type |= RV_ST_TESSELLATION_CONTROL;
	if (strstr(path, ".tese"))
		type |= RV_ST_TESSELLATION_EVALUATION;
	if (strstr(path, ".geom"))
		type |= RV_ST_GEOMETRY;
	if (strstr(path, ".frag"))
		type |= RV_ST_FRAGMENT;
	if (strstr(path, ".comp"))
		type |= RV_ST_COMPUTE;
	if (strstr(path, ".mesh"))
		type |= RV_ST_MESH;
	if (strstr(path, ".task"))
		type |= RV_ST_TASK;
	if (strstr(path, ".rgen"))
		type |= RV_ST_RAYGEN;
	if (strstr(path, ".rint"))
		type |= RV_ST_INTERSECTION;
	if (strstr(path, ".rahit"))
		type |= RV_ST_ANY_HIT;
	if (strstr(path, ".rchit"))
		type |= RV_ST_CLOSEST_HIT;
	if (strstr(path, ".rmiss"))
		type |= RV_ST_MISS;
	if (strstr(path, ".rcall"))
		type |= RV_ST_CALLABLE;
	return type;
}
