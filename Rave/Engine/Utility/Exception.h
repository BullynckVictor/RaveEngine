#pragma once
#include "Engine/Utility/Result.h"
#include <stdexcept>

namespace rv
{
	struct ErrorInfo : public ResultInfo
	{
	public:
		ErrorInfo();
		ErrorInfo(const std::string& description, const std::vector<std::string>& info = {});
		ErrorInfo(std::string&& description, std::vector<std::string>&& info = {});
		ErrorInfo(const char* source, u64 line, const std::string& description, const std::vector<std::string>& info = {});
		ErrorInfo(const char* source, u64 line, std::string&& description, std::vector<std::string>&& info = {});
		virtual ~ErrorInfo() = default;

		u64 line() const;
		const char* source() const;
		bool has_source_line_info() const;

	protected:
		u64 m_line;
		const char* m_source;
	};

	class ResultException : public std::runtime_error
	{
	public:
		ResultException();
		ResultException(const Result& result, const std::string& message = {});

		const Result& result() const;

	private:
		std::string format(const Result& result, const std::string& message) const;
		Result m_result = {};
	};
}