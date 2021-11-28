#pragma once
#include <filesystem>

namespace rv
{
	static bool FileExists(const char* filename)
	{
		return std::filesystem::exists(filename);
	}
	static bool FileExists(const std::filesystem::path& filename)
	{
		return std::filesystem::exists(filename);
	}
}