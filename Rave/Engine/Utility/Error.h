#pragma once
#include "Engine/Utility/Exception.h"
#include "Engine/Graphics/Vulkan.h"

#undef assert

namespace rv
{
#	ifdef RV_KEEP_INFO_ON_SUCCESS
	static constexpr bool keep_info_on_success = true;
#	else
	static constexpr bool keep_info_on_success = false;
#	endif

	struct Assertion : public ErrorInfo
	{
		Assertion() = default;
		Assertion(bool condition, const std::string& info = {}, const char* cond_str = nullptr);
		Assertion(const char* source, u64 line, bool condition, const std::string& info, const char* cond_str = nullptr);

		bool condition;
		const char* cond_str;
	};

	static constexpr ResultCode failed_assertion = ResultCode("Failed Assertion", RV_SEVERITY_ERROR);
	static constexpr ResultCode succeeded_assertion = ResultCode("Succeeded Assertion", RV_SEVERITY_INFO);

	Result assert(bool condition);
	Result assert(bool condition, const char* str, const char* source, u64 line);
	Result assert(bool condition, const std::string& info);
	Result assert(bool condition, const char* str, const char* source, u64 line, const std::string& info);


	struct VulkanResult : public ErrorInfo
	{
		VulkanResult() = default;
		VulkanResult(VkResult result, const std::string& info = {});
		VulkanResult(const char* source, u64 line, VkResult result, const std::string& info = {});

		VkResult result;
	};

	static constexpr ResultCode failed_vkr = ResultCode("Failed Vulkan Result", RV_SEVERITY_ERROR);
	static constexpr ResultCode succeeded_vkr = ResultCode("Succeeded Vulkan Result", RV_SEVERITY_INFO);

	Result try_vkr(VkResult result);
	Result try_vkr(VkResult result, const char* source, u64 line);
	Result try_vkr(VkResult result, const std::string& info);
	Result try_vkr(VkResult result, const char* source, u64 line, const std::string& info);

	Result assert_vkr(VkResult result);
	Result assert_vkr(VkResult result, const char* source, u64 line);
	Result assert_vkr(VkResult result, const std::string& info);
	Result assert_vkr(VkResult result, const char* source, u64 line, const std::string& info);
}

#define RV_FILE_LINE			__FILE__, __LINE__
#define rv_result				rv::Result result = rv::success;
#define rv_rif(res)				if (((result) = (res)).failed()) return result

#define rv_try_vkr(vkr)						rv::try_vkr(vkr, RV_FILE_LINE)
#define rv_try_vkr_info(vkr,msg)			rv::try_vkr(vkr, RV_FILE_LINE, msg)

#define rif_try_vkr(vkr)					rv_rif(rv_try_vkr(vkr))
#define rif_try_vkr_info(vkr, msg)			rv_rif(rv_try_vkr_info(vkr, msg))

#ifdef RV_DEBUG
#	define rv_assert(cond)					(rv::assert)(cond, #cond, RV_FILE_LINE)
#	define rv_assert_info(cond, msg)		(rv::assert)(cond, #cond, RV_FILE_LINE, msg)

#	define rif_assert(cond)					rv_rif(rv_assert(cond))
#	define rif_assert_info(cond, msg)		rv_rif(rv_assert_info(cond, msg))


#	define rv_assert_vkr(res)				rv::assert_vkr(res, RV_FILE_LINE)
#	define rv_assert_vkr_info(res, msg)		rv::assert_vkr(res, RV_FILE_LINE, msg)

#	define rif_assert_vkr(res)				rv_rif(rv_assert(res))
#	define rif_assert_vkr_info(res, msg)	rv_rif(rv_assert_info(res, msg))
#else
#	define rv_assert(cond)
#	define rv_assert_info(cond, msg)

#	define rif_assert(cond)
#	define rif_assert_info(cond, msg)


#	define rv_assert_vkr(res)
#	define rv_assert_vkr_info(res, msg)

#	define rif_assert_vkr(res)
#	define rif_assert_vkr_info(res, msg)
#endif