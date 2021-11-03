#include "Engine/Utility/Exception.h"
#include "Engine/Utility/String.h"

rv::ErrorInfo::ErrorInfo()
	:
	ResultInfo(),
	m_line(0),
	m_source(nullptr)
{
}

rv::ErrorInfo::ErrorInfo(const std::string& description, const std::vector<std::string>& info)
	:
	ResultInfo(description, info),
	m_line(0),
	m_source(nullptr)
{
}

rv::ErrorInfo::ErrorInfo(std::string&& description, std::vector<std::string>&& info)
	:
	ResultInfo(std::move(description), std::move(info)),
	m_line(0),
	m_source(nullptr)
{
}

rv::ErrorInfo::ErrorInfo(const char* source, u64 line, const std::string& description, const std::vector<std::string>& info)
	:
	ResultInfo(description),
	m_line(line),
	m_source(source)
{
	m_info.reserve(info.size() + 2);
	m_info.push_back(str("Source file: ", source));
	m_info.push_back(str("Line: ", line));
	m_info.insert(m_info.end(), info.begin(), info.end());
}

rv::ErrorInfo::ErrorInfo(const char* source, u64 line, std::string&& description, std::vector<std::string>&& info)
	:
	ResultInfo(std::move(description)),
	m_line(line),
	m_source(source)
{
	m_info.reserve(info.size() + 2);
	m_info.push_back(str("Source file: ", source));
	m_info.push_back(str("Line: ", line));
	m_info.insert(m_info.end(), info.begin(), info.end());
}

rv::u64 rv::ErrorInfo::line() const
{
	return m_line;
}

const char* rv::ErrorInfo::source() const
{
	return m_source;
}

bool rv::ErrorInfo::has_source_line_info() const
{
	return m_source;
}

rv::ResultException::ResultException()
	:
	std::runtime_error(nullptr),
	m_result()
{
}

rv::ResultException::ResultException(const Result& result, const std::string& message)
	:
	std::runtime_error(format(result, message)),
	m_result(result)
{
}

const rv::Result& rv::ResultException::result() const
{
	return m_result;
}

std::string rv::ResultException::format(const Result& result, const std::string& message) const
{
	std::ostringstream ss;

	ss << "rv::ResultException occurred!\n\n";
	ss << "Type: " << result.code().name() << "\n";
	ss << "Severity: " << make_string(result.severity());

	if (result.has_info())
	{

		const ResultInfo& info = result.info();
		bool has_info = !info.info().empty();
		bool has_description = !info.description().empty();

		if (has_info)
		{
			ss << "\n";
			for (size_t i = 0; i < info.info().size(); ++i)
				ss << info.info()[i] << (i == info.info().size() - 1 ? nullptr : "\n");
		}

		if (has_description)
			ss << '\n' << '\n' << "Description: " << info.description();

	}

	bool has_message = !message.empty();
	if (has_message)
		ss << '\n' << '\n' << "Message: " << message;

	return ss.str();
}
