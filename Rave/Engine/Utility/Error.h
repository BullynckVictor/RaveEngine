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

	static constexpr ResultCode runtime_error = ResultCode("Runtime Error", RV_SEVERITY_ERROR);
	Result make_runtime_error(const std::string& error);
	Result make_runtime_error(const std::string& error, const char* source, u64 line);

	struct Condition : public ErrorInfo
	{
		Condition() = default;
		Condition(bool condition, const std::string& info = {}, const char* cond_str = nullptr);
		Condition(const char* source, u64 line, bool condition, const std::string& info, const char* cond_str = nullptr);

		bool condition;
		const char* cond_str;
	};

	struct FileInfo : public ErrorInfo
	{
		FileInfo() = default;
		FileInfo(const std::string& filename, const std::string& info = {});
		FileInfo(const char* source, u64 line, const std::string& filename, const std::string& info = {});

		std::string filename;
	};

	static constexpr ResultCode failed_file = ResultCode("Failed File", RV_SEVERITY_ERROR);
	static constexpr ResultCode succeeded_file = ResultCode("Succeeded File", RV_SEVERITY_INFO);

	Result try_file(const char* filename);
	Result try_file(const char* filename, const char* source, u64 line);
	Result try_file(const char* filename, const std::string& info);
	Result try_file(const char* filename, const char* source, u64 line, const std::string& info);

	static constexpr ResultCode failed_assertion = ResultCode("Failed Assertion", RV_SEVERITY_ERROR);
	static constexpr ResultCode succeeded_assertion = ResultCode("Succeeded Assertion", RV_SEVERITY_INFO);

	Result assert(bool condition);
	Result assert(bool condition, const char* str, const char* source, u64 line);
	Result assert(bool condition, const std::string& info);
	Result assert(bool condition, const char* str, const char* source, u64 line, const std::string& info);

	static constexpr ResultCode failed_condition = ResultCode("Failed Condition", RV_SEVERITY_ERROR);
	static constexpr ResultCode succeeded_condition = ResultCode("Succeeded Condition", RV_SEVERITY_INFO);

	Result check(bool condition);
	Result check(bool condition, const char* str, const char* source, u64 line);
	Result check(bool condition, const std::string& info);
	Result check(bool condition, const char* str, const char* source, u64 line, const std::string& info);


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

#	ifdef RV_PLATFORM_WINDOWS
	namespace win32
	{
		struct HrResult : public ErrorInfo
		{
			HrResult() = default;
			HrResult(HRESULT result, const std::string& info = {});
			HrResult(const char* source, u64 line, HRESULT result, const std::string& info = {});

			HRESULT result;
		};

		static constexpr ResultCode failed_hr = ResultCode("Failed HRESULT", RV_SEVERITY_ERROR);
		static constexpr ResultCode succeeded_hr = ResultCode("Succeeded HRESULT", RV_SEVERITY_INFO);

		Result try_hr(HRESULT result);
		Result try_hr(HRESULT result, const char* source, u64 line);
		Result try_hr(HRESULT result, const std::string& info);
		Result try_hr(HRESULT result, const char* source, u64 line, const std::string& info);

		Result assert_hr(HRESULT result);
		Result assert_hr(HRESULT result, const char* source, u64 line);
		Result assert_hr(HRESULT result, const std::string& info);
		Result assert_hr(HRESULT result, const char* source, u64 line, const std::string& info);

		Result check(bool condition);
		Result check(bool condition, const char* source, u64 line);
		Result check(bool condition, const std::string& info);
		Result check(bool condition, const char* source, u64 line, const std::string& info);

		Result assert(bool condition);
		Result assert(bool condition, const char* source, u64 line);
		Result assert(bool condition, const std::string& info);
		Result assert(bool condition, const char* source, u64 line, const std::string& info);
	}
#	endif
}

#define RV_FILE_LINE			__FILE__, __LINE__
#define rv_result				rv::Result result = rv::success;
#define rv_rif(res)				if (((result) = (res)).failed()) return result

#define rv_runtime_error(msg)					rv::make_runtime_error(msg)
#define rif_runtime_error(msg)					rv_rif(rv_runtime_error(msg))
#define rv_throw(msg)							rv_runtime_error(msg).throw_exception()

#define rv_try_vkr(vkr)							rv::try_vkr(vkr, RV_FILE_LINE)
#define rv_try_vkr_info(vkr,msg)				rv::try_vkr(vkr, RV_FILE_LINE, msg)

#define rif_try_vkr(vkr)						rv_rif(rv_try_vkr(vkr))
#define rif_try_vkr_info(vkr, msg)				rv_rif(rv_try_vkr_info(vkr, msg))


#define rv_try_file(filename)					rv::try_file(filename, RV_FILE_LINE)
#define rv_try_file_info(filename, msg)			rv::try_file(filename, RV_FILE_LINE, msg)

#define rif_try_file(filename)					rv_rif(rv_try_file(filename))
#define rif_try_file_info(filename, msg)		rv_rif(rv_try_file_info(filename, msg))


#define rv_check(cond)							rv::check(cond, #cond, RV_FILE_LINE)
#define rv_check_info(cond, msg)				rv::check(cond, #cond, RV_FILE_LINE, msg)

#define rif_check(cond)							rv_rif(rv_check(cond))
#define rif_check_info(cond, msg)				rv_rif(rv_check_info(cond, msg))


#ifdef RV_PLATFORM_WINDOWS

#define rv_try_hr(hr)							rv::win32::try_hr(hr, RV_FILE_LINE)
#define rv_try_hr_info(hr, msg)					rv::win32::try_hr(hr, RV_FILE_LINE, msg)

#define rif_try_hr(hr)							rv_rif(rv_try_hr(hr))
#define rif_try_hr_info(hr, msg)				rv_rif(rv_try_hr_info(hr, msg))


#define rv_win32_check(cond)					rv::win32::check(cond, RV_FILE_LINE)
#define rv_win32_check_info(cond, msg)			rv::win32::check(cond, RV_FILE_LINE, msg)

#define rif_win32_check(cond)					rv_rif(rv_win32_check(cond))
#define rif_win32_check_info(cond, msg)			rv_rif(rv_win32_check_info(cond, msg))

#endif


#ifdef RV_DEBUG
#	define rv_assert(cond)						(rv::assert)(cond, #cond, RV_FILE_LINE)
#	define rv_assert_info(cond, msg)			(rv::assert)(cond, #cond, RV_FILE_LINE, msg)

#	define rif_assert(cond)						rv_rif(rv_assert(cond))
#	define rif_assert_info(cond, msg)			rv_rif(rv_assert_info(cond, msg))


#	define rv_assert_vkr(res)					rv::assert_vkr(res, RV_FILE_LINE)
#	define rv_assert_vkr_info(res, msg)			rv::assert_vkr(res, RV_FILE_LINE, msg)

#	define rif_assert_vkr(res)					rv_rif(rv_assert_vkr(res))
#	define rif_assert_vkr_info(res, msg)		rv_rif(rv_assert_vkr_info(res, msg))

#ifdef RV_PLATFORM_WINDOWS

#	define rv_assert_hr(hr)						rv::win32::assert_hr(hr, RV_FILE_LINE)
#	define rv_assert_hr_info(hr, msg)			rv::win32::assert_hr(hr, RV_FILE_LINE, msg)

#	define rif_assert_hr(hr)					rv_rif(rv_assert_hr(hr))
#	define rif_assert_hr_info(hr, msg)			rv_rif(rv_assert_hr_info(hr, msg))


#	define rv_win32_assert(cond)				rv::win32::assert(cond, RV_FILE_LINE)
#	define rv_win32_assert_info(cond, msg)		rv::win32::assert(cond, RV_FILE_LINE, msg)

#	define rif_win32_assert(cond)				rv_rif(rv_win32_assert(cond))
#	define rif_win32_assert_info(cond, msg)		rv_rif(rv_win32_assert_info(cond, msg))

#endif
#else
#	define rv_assert(cond)
#	define rv_assert_info(cond, msg)

#	define rif_assert(cond)
#	define rif_assert_info(cond, msg)


#	define rv_assert_vkr(res)
#	define rv_assert_vkr_info(res, msg)

#	define rif_assert_vkr(res)
#	define rif_assert_vkr_info(res, msg)


#ifdef RV_PLATFORM_WINDOWS

#	define rv_assert_hr(hr)
#	define rv_assert_hr_info(hr, msg)

#	define rif_assert_hr(hr)
#	define rif_assert_hr_info(hr, msg)


#	define rv_win32_assert(cond)
#	define rv_win32_assert_info(cond, msg)

#	define rif_win32_assert(cond)
#	define rif_win32_assert_info(cond, msg)

#endif
#endif