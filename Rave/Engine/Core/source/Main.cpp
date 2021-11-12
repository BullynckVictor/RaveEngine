#include <iostream>
#include "Engine/Core/Main.h"
#include "Engine/Utility/Exception.h"
#include "Engine/Core/SystemInclude.h"
#include "Engine/Graphics/DebugMessenger.h"

void message_box(const char* title, const char* text, rv::Severity severity)
{
	if constexpr (rv::CompileTimeInfo::Platform::windows)
	{
		UINT type = MB_OK;
		switch (severity)
		{
			case rv::RV_SEVERITY_INFO:
				type |= MB_ICONINFORMATION;
				break;

			case rv::RV_SEVERITY_WARNING:
				type |= MB_ICONWARNING;
				break;

			case rv::RV_SEVERITY_ERROR:
			default:
				type |= MB_ICONERROR;
				break;
		}
		MessageBox(nullptr, text, title, type);
	}
}

int main()
{
	rv::Result result = rv::success;
	try
	{
		result = rave_main();
		if (result.fatal())
		{
			message_box("rv::ResultException", result.exception("Application failed: 'rave_main()' returned fatal result").what(), result.severity());
		}
		else
		{
			result = rv::DebugMessenger::StaticCheck();
			if (result.fatal())
			{
				message_box("rv::ResultException", result.exception("Application failed: Vulkan Debug Messenger returned fatal result").what(), result.severity());
				rv::DebugMessenger::StaticFlush();
			}
			else
			{
				rv::DebugMessenger::StaticFlush();
				std::cin.ignore();
			}
		}
	}
	catch (const rv::ResultException& e)
	{
		message_box("rv::ResultException", e.what(), e.result().severity());
	}
	catch (const std::exception& e)
	{
		message_box("std::exception", e.what(), rv::RV_SEVERITY_ERROR);
	}
	catch (...)
	{
		message_box("Unknown exception", "Something went wrong, please contact the developper(s)", rv::RV_SEVERITY_ERROR);
	}
	return result.fatal() ? EXIT_FAILURE : EXIT_SUCCESS;
}