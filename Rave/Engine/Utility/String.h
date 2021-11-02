#pragma once
#include <string>
#include <sstream>

namespace rv
{
	namespace detail
	{
		template<typename C, typename T, typename... Args >
		void str(std::basic_stringstream<C>& ss, const T& value, const Args&... args)
		{
			ss << value;

			if constexpr (sizeof...(Args) != 0)
				str<C>(ss, args...);
		}
	}

	template<typename... Args>
	std::string str(const Args&... args)
	{
		return str_t<char>(args...);
	}

	template<typename C, typename... Args>
	std::basic_string<C> str_t(const Args&... args)
	{
		if constexpr (sizeof...(Args) == 0)
			return {};
		else
		{
			std::basic_stringstream<C> ss{};
			detail::str<C>(ss, args...);
			return ss.str();
		}
	}
}