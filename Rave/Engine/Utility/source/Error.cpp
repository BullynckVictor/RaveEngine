#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"
#include "Engine/Utility/VkResult.h"

rv::Assertion::Assertion(bool condition, const std::string& info, const char* cond_str)
	:
	ErrorInfo(
		str("Assertion ", cond_str ? str("\"", cond_str, "\" ") : std::string(), condition ? " succeeded" : " failed", info.empty() ? "" : "\n" + info)
	),
	condition(condition),
	cond_str(cond_str)
{
}

rv::Assertion::Assertion(const char* source, u64 line, bool condition, const std::string& info, const char* cond_str)
	:
	ErrorInfo(
		source, line, str("Assertion ", cond_str ? str("\"", cond_str, "\" ") : std::string(), condition ? " succeeded" : " failed", info.empty() ? "" : "\n" + info)
	),
	condition(condition),
	cond_str(cond_str)
{
}

rv::Result rv::assert(bool condition)
{
	if constexpr (cti.build.debug)
	{
		if (condition)
			if constexpr (keep_info_on_success)
				return succeeded_assertion;
			else
				return Result(succeeded_assertion, new Assertion(condition));
		else
			return Result(failed_assertion, new Assertion(condition));
	}
	else
		return succeeded_assertion;
}

rv::Result rv::assert(bool condition, const char* str, const char* source, u64 line)
{
	if constexpr (cti.build.debug)
	{
		if (condition)
			if constexpr (keep_info_on_success)
				return succeeded_assertion;
			else
				return Result(succeeded_assertion, new Assertion(source, line, condition, {}, str));
		else
			return Result(failed_assertion, new Assertion(source, line, condition, {}, str));
	}
	else
		return succeeded_assertion;
}

rv::Result rv::assert(bool condition, const std::string& info)
{
	if constexpr (cti.build.debug)
	{
		if (condition)
			if constexpr (keep_info_on_success)
				return succeeded_assertion;
			else
				return Result(succeeded_assertion, new Assertion(condition, info));
		else
			return Result(failed_assertion, new Assertion(condition, info));
	}
	else
		return succeeded_assertion;
}

rv::Result rv::assert(bool condition, const char* str, const char* source, u64 line, const std::string& info)
{
	if constexpr (cti.build.debug)
	{
		if (condition)
			if constexpr (keep_info_on_success)
				return succeeded_assertion;
			else
				return Result(succeeded_assertion, new Assertion(source, line, condition, info, str));
		else
			return Result(failed_assertion, new Assertion(source, line, condition, info, str));
	}
	else
		return succeeded_assertion;
}

rv::VulkanResult::VulkanResult(VkResult result, const std::string& info)
	:
	ErrorInfo(str(description_short(result), ": ", rv::description(result), info.empty() ? "" : "\n" + info), { str("VkResult: ", result) }),
	result(result)
{
}

rv::VulkanResult::VulkanResult(const char* source, u64 line, VkResult result, const std::string& info)
	:
	ErrorInfo(source, line, str(description_short(result), ": ", rv::description(result), info.empty() ? "" : "\n" + info), { str("VkResult: ", result) }),
	result(result)
{
}

rv::Result rv::try_vkr(VkResult result)
{
	if (failed(result))
		return Result(failed_vkr, new VulkanResult(result));
	return Result(succeeded_vkr, keep_info_on_success ? new VulkanResult(result) : nullptr);
}

rv::Result rv::try_vkr(VkResult result, const char* source, u64 line)
{
	if (failed(result))
		return Result(failed_vkr, new VulkanResult(source, line, result));
	return Result(succeeded_vkr, keep_info_on_success ? new VulkanResult(source, line, result) : nullptr);
}

rv::Result rv::try_vkr(VkResult result, const std::string& info)
{
	if (failed(result))
		return Result(failed_vkr, new VulkanResult(result, info));
	return Result(succeeded_vkr, keep_info_on_success ? new VulkanResult(result, info) : nullptr);
}

rv::Result rv::try_vkr(VkResult result, const char* source, u64 line, const std::string& info)
{
	if (failed(result))
		return Result(failed_vkr, new VulkanResult(source, line, result, info));
	return Result(succeeded_vkr, keep_info_on_success ? new VulkanResult(source, line, result, info) : nullptr);
}

rv::Result rv::assert_vkr(VkResult result)
{
	if constexpr (cti.build.debug)
		return try_vkr(result);
	return succeeded_vkr;
}

rv::Result rv::assert_vkr(VkResult result, const char* source, u64 line)
{
	if constexpr (cti.build.debug)
		return try_vkr(result, source, line);
	return succeeded_vkr;
}

rv::Result rv::assert_vkr(VkResult result, const std::string& info)
{
	if constexpr (cti.build.debug)
		return try_vkr(result, info);
	return succeeded_vkr;
}

rv::Result rv::assert_vkr(VkResult result, const char* source, u64 line, const std::string& info)
{
	if constexpr (cti.build.debug)
		return try_vkr(result, source, line, info);
	return succeeded_vkr;
}
