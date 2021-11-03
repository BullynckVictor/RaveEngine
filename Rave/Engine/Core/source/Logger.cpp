#include "Engine/Core/Logger.h"
#include <iomanip>
#ifdef RV_DEBUG_LOGGER
#include <iostream>
#include <fstream>
#include "Engine/Core/SystemInclude.h"
#endif

rv::DebugLogger rv::debug;

rv::Message::Message()
	:
	time(std::chrono::system_clock::now()),
	severity(RV_SEVERITY_INFO)
{
}

rv::Message::Message(const std::string& message, Severity severity)
	:
	time(std::chrono::system_clock::now()),
	severity(severity),
	message(message)
{
}

rv::Message::Message(std::string&& message, Severity severity)
	:
	time(std::chrono::system_clock::now()),
	severity(severity),
	message(std::move(message))
{
}

std::string rv::Message::format(const char* date_fmt) const
{
	std::ostringstream ss;
	switch (severity)
	{
		case RV_SEVERITY_INFO:		ss << "[Info]     "; break;
		case RV_SEVERITY_WARNING:	ss << "[Warning]  "; break;
		case RV_SEVERITY_ERROR:		ss << "[Error]    "; break;
	}

	time_t t = std::chrono::system_clock::to_time_t(time);
	tm tm{};
	errno_t e = localtime_s(&tm, &t);
	ss << std::put_time(&tm, date_fmt) << "    " << message;
	return ss.str();
}

rv::Logger::Logger(Flags<Severity> allowedSeverity)
	:
	allowedSeverity(allowedSeverity)
{
}

void rv::Logger::Log(const Message& message)
{
	if (allowedSeverity.contains(message.severity))
	{
		messages.push_back(message);
		OnLog(message);
	}
}

void rv::Logger::Log(const std::string& message, Severity severity)
{
	Log(Message(message, severity));
}

void rv::Logger::Log(std::string&& message, Severity severity)
{
	Log(Message(std::move(message), severity));
}

#ifdef RV_DEBUG_LOGGER

rv::DebugLogger::DebugLogger(Flags<Severity> allowedSeverity, const char* dumpFile)
	:
	Logger(allowedSeverity),
	dumpFile(dumpFile)
{
}

rv::DebugLogger::~DebugLogger()
{
	if (dumpFile)
	{
		std::ofstream file(dumpFile);
		if (file.is_open())
			for (const auto& m : messages)
				file << m.format() << '\n';
	}
}

void rv::DebugLogger::OnLog(const Message& message)
{
	std::string fmt = message.format();
	fmt.push_back('\n');
	std::cout << fmt;
	if constexpr (cti.platform.windows)
		OutputDebugString(fmt.c_str());
}

#else

rv::DebugLogger::DebugLogger(Flags<Severity> allowedSeverity, const char* dumpFile)
	:
	allowedSeverity(allowedSeverity),
	dumpFile(dumpFile)
{
}

void rv::DebugLogger::Log(const Message& message)
{
}

void rv::DebugLogger::Log(const std::string& message, Severity severity)
{
}

void rv::DebugLogger::Log(std::string&& message, Severity severity)
{
}

#endif