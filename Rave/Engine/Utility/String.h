#pragma once
#include <string>
#include <sstream>

namespace rv
{
	template<typename T>
	static constexpr const char* make_string(const T& value) { return nullptr; }

	namespace detail
	{
		template<typename C, typename T, typename... Args >
		static void str(std::basic_ostringstream<C>& ss, const T& value, const Args&... args)
		{
			const char* string = make_string(value);
			if (string)
				ss << string;
			else
				ss << value;

			if constexpr (sizeof...(Args) != 0)
				str<C>(ss, args...);
		}
	}

	template<typename C, typename... Args>
	static std::basic_string<C> str_t(const Args&... args)
	{
		if constexpr (sizeof...(Args) == 0)
			return {};
		else
		{
			std::basic_ostringstream<C> ss{};
			detail::str<C>(ss, args...);
			return ss.str();
		}
	}

	template<typename... Args>
	static std::string str(const Args&... args)
	{
		return str_t<char>(args...);
	}

	static bool strcomp(const char* a, const char* b)
	{
		return !strcmp(a, b);
	}
}