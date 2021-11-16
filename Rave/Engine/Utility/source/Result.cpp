#include "Engine/Utility/Result.h"
#include "Engine/Utility/Exception.h"

rv::ResultInfo::ResultInfo(const std::string& description, const std::vector<std::string>& info)
	:
	m_description(description),
	m_info(info)
{
}

rv::ResultInfo::ResultInfo(std::string&& description, std::vector<std::string>&& info)
	:
	m_description(std::move(description)),
	m_info(std::move(info))
{
}

const std::string& rv::ResultInfo::description() const
{
	return m_description;
}

const std::vector<std::string>& rv::ResultInfo::info() const
{
	return m_info;
}

rv::Result::Result(ResultCode code, ResultInfo* info)
	:
	m_code(code),
	m_info(info)
{
}

rv::ResultCode rv::Result::code() const
{
	return m_code;
}

rv::Severity rv::Result::severity() const
{
	return m_code.severity();
}

bool rv::Result::succeeded(Flags<Severity> severity) const
{
	return severity.contain(m_code.severity());
}

bool rv::Result::failed(Flags<Severity> severity) const
{
	return severity.contain(m_code.severity());
}

bool rv::Result::fatal() const
{
	return failed(RV_SEVERITY_ERROR);
}

bool rv::Result::has_info() const
{
	return m_info.get();
}

const rv::ResultInfo& rv::Result::info() const
{
	return *m_info.get();
}

const rv::ResultInfo* rv::Result::info_pointer() const
{
	return m_info.get();
}

rv::ResultException rv::Result::exception(const std::string& message) const
{
	return ResultException(*this, message);
}

void rv::Result::throw_exception(const std::string& message) const
{
	throw exception(message);
}

void rv::Result::expect(const std::string& message)
{
	expect(RV_SEVERITY_INFO, message);
}

void rv::Result::expect(Flags<Severity> severity, const std::string& message)
{
	if (!succeeded(severity))
		throw_exception(message);
}
