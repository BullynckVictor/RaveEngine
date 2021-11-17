#pragma once
#include "Engine/Core/CompileTimeInfo.h"
#include "Engine/Utility/Result.h"
#include "Engine/Utility/Flags.h"
#include "Engine/Utility/UnknownObject.h"
#include <deque>
#include <chrono>
#include <ios>
#include <mutex>

namespace rv
{
	struct Message
	{
		Message();
		Message(const std::string& message, Severity severity = RV_SEVERITY_INFO);
		Message(std::string&& message, Severity severity = RV_SEVERITY_INFO);

		std::string format(const char* date_fmt = "%H:%M:%S") const;

		Severity severity;
		std::chrono::system_clock::time_point time;
		std::string message;
	};

	class Logger
	{
	public:
		Logger(Flags<Severity> allowedSeverity = combine(RV_SEVERITY_INFO, RV_SEVERITY_WARNING, RV_SEVERITY_ERROR));
		virtual ~Logger() = default;

		void Log(const Message& message);
		void Log(const std::string& message, Severity severity = RV_SEVERITY_INFO);
		void Log(std::string&& message, Severity severity = RV_SEVERITY_INFO);

		Flags<Severity> allowedSeverity;

	protected:
		virtual void OnLog(const Message& message) {}

	protected:
		std::deque<Message> messages;
		std::mutex mutex;
	};
}


#if defined(RV_DEBUG_LOGGER) && defined(RV_NO_DEBUG_LOGGER)
#	error RV_DEBUG_LOGGER and RV_NO_DEBUG_LOGGER cannot both be defined
#endif
#ifndef RV_NO_DEBUG_LOGGER
#ifdef RV_DEBUG
#	define RV_DEBUG_LOGGER
#endif
#endif

namespace rv
{
#ifdef RV_DEBUG_LOGGER
	class DebugLogger : public Logger
	{
	public:
		DebugLogger(Flags<Severity> allowedSeverity = combine(RV_SEVERITY_INFO, RV_SEVERITY_WARNING, RV_SEVERITY_ERROR), const char* dumpFile = "Application/log.txt");
		~DebugLogger();

		const char* dumpFile;

	protected:
		void OnLog(const Message& message) override;
	};
#else
	class DebugLogger
	{
	public:
		DebugLogger(Flags<Severity> allowedSeverity = combine(RV_SEVERITY_INFO, RV_SEVERITY_WARNING, RV_SEVERITY_ERROR), const char* dumpFile = "Application/log.txt");

		void Log(const Message& message);
		void Log(const std::string& message, Severity severity = RV_SEVERITY_INFO);
		void Log(std::string&& message, Severity severity = RV_SEVERITY_INFO);

		Flags<Severity> allowedSeverity;
		const char* dumpFile;
	};
#endif

	extern DebugLogger debug;
}

#ifdef RV_DEBUG_LOGGER
#define rv_log(msg)					rv::debug.Log(msg)
#define rv_debug_logger_only(code)	code
#else
#define rv_log(msg)
#define rv_debug_logger_only(code)
#endif