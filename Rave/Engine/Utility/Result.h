#pragma once
#include "Engine/Utility/Flags.h"
#include "Engine/Utility/Hash.h"
#include <string>
#include <vector>
#include <memory>

namespace rv
{
	enum Severity : u8
	{
		RV_SEVERITY_INFO = 0,
		RV_SEVERITY_WARNING = 1,
		RV_SEVERITY_ERROR = 2,
	};

	class ResultException;

	struct ResultCode
	{
	public:
		constexpr ResultCode() : m_hash_severity(0), m_name(nullptr) {}
		constexpr ResultCode(const char* name, const Severity severity) : m_hash_severity(hash_severity(name, severity)), m_name(name) {}
		constexpr ResultCode(const ResultCode& rhs) : m_hash_severity(rhs.m_hash_severity), m_name(rhs.m_name) {}
		constexpr ResultCode(ResultCode&& rhs) noexcept : m_hash_severity(rhs.m_hash_severity), m_name(rhs.m_name) { rhs.m_hash_severity = 0; rhs.m_name = nullptr; }

		constexpr ResultCode& operator= (const ResultCode& rhs) { m_hash_severity = rhs.m_hash_severity; m_name = rhs.m_name; return *this; }
		constexpr ResultCode& operator= (ResultCode&& rhs) noexcept { m_hash_severity = rhs.m_hash_severity; m_name = rhs.m_name; rhs.m_hash_severity = 0; rhs.m_name = nullptr; return *this; }

		constexpr bool operator== (const ResultCode& rhs) const { return m_hash_severity == rhs.m_hash_severity; }
		constexpr bool operator!= (const ResultCode& rhs) const { return m_hash_severity != rhs.m_hash_severity; }
		constexpr bool operator<= (const ResultCode& rhs) const { return m_hash_severity <= rhs.m_hash_severity; }
		constexpr bool operator>= (const ResultCode& rhs) const { return m_hash_severity >= rhs.m_hash_severity; }
		constexpr bool operator<  (const ResultCode& rhs) const { return m_hash_severity <  rhs.m_hash_severity; }
		constexpr bool operator>  (const ResultCode& rhs) const { return m_hash_severity >  rhs.m_hash_severity; }

		constexpr const char* name() const { return m_name; }
		constexpr Severity severity() const { return Severity(m_hash_severity & 0b11); }
		
		constexpr bool valid() const { return m_name; }
		constexpr bool invalid() const { return !m_name; }

	private:
		static constexpr size_t hash_severity(const char* name, const Severity severity)
		{
			size_t t = (size_t)0xFFFFFFFFFFFFFFFF ^ (size_t)0b11;
			return hash(name) & t | severity;
		}
		
		size_t m_hash_severity;
		const char* m_name;
	};

	struct ResultInfo
	{
	public:
		ResultInfo() = default;
		ResultInfo(const std::string& description, const std::vector<std::string>& info = {});
		ResultInfo(std::string&& description, std::vector<std::string>&& info = {});
		virtual ~ResultInfo() = default;

		const std::string& description() const;
		const std::vector<std::string>& info() const;

	protected:
		std::string m_description;
		std::vector<std::string> m_info;
	};

	struct Result
	{
	public:
		Result() = default;
		Result(ResultCode code, ResultInfo* info = nullptr);

		ResultCode code() const;
		Severity severity() const;

		bool succeeded(Flags<Severity> severity = RV_SEVERITY_INFO) const;
		bool failed(Flags<Severity> severity = RV_SEVERITY_WARNING | RV_SEVERITY_ERROR) const;
		bool fatal() const;

		bool has_info() const;
		const ResultInfo& info() const;
		const ResultInfo* info_pointer() const;

		ResultException exception(const std::string& message = {}) const;
		void throw_exception(const std::string& message = {}) const;
		void expect(const std::string& message = {});
		void expect(Flags<Severity> severity, const std::string& message = {});

	private:
		ResultCode m_code;
		std::shared_ptr<ResultInfo> m_info;
	};

	static constexpr const char* make_string(Severity severity)
	{
		switch (severity)
		{
			case RV_SEVERITY_INFO:		return "Info";
			case RV_SEVERITY_WARNING:	return "Warning";
			case RV_SEVERITY_ERROR:		return "Error";
			default:					return "???";
		}
	}

	static constexpr ResultCode success = ResultCode("Success", RV_SEVERITY_INFO);
	static constexpr ResultCode failure = ResultCode("Failure", RV_SEVERITY_ERROR);
}