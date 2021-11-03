#pragma once
#include "Engine/Utility/String.h"
#include "Engine/Graphics/Vulkan.h"

namespace rv
{
	template<>
	static constexpr const char* make_string(const VkResult& result)
	{
		switch (result)
		{
			case VK_SUCCESS:													return "VK_SUCCESS";
			case VK_NOT_READY:													return "VK_NOT_READY";
			case VK_TIMEOUT:													return "VK_TIMEOUT";
			case VK_EVENT_SET:													return "VK_EVENT_SET";
			case VK_EVENT_RESET:												return "VK_EVENT_RESET";
			case VK_INCOMPLETE:													return "VK_INCOMPLETE";
			case VK_ERROR_OUT_OF_HOST_MEMORY:									return "VK_ERROR_OUT_OF_HOST_MEMORY";
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:									return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
			case VK_ERROR_INITIALIZATION_FAILED:								return "VK_ERROR_INITIALIZATION_FAILED";
			case VK_ERROR_DEVICE_LOST:											return "VK_ERROR_DEVICE_LOST";
			case VK_ERROR_MEMORY_MAP_FAILED:									return "VK_ERROR_MEMORY_MAP_FAILED";
			case VK_ERROR_LAYER_NOT_PRESENT:									return "VK_ERROR_LAYER_NOT_PRESENT";
			case VK_ERROR_EXTENSION_NOT_PRESENT:								return "VK_ERROR_EXTENSION_NOT_PRESENT";
			case VK_ERROR_FEATURE_NOT_PRESENT:									return "VK_ERROR_FEATURE_NOT_PRESENT";
			case VK_ERROR_INCOMPATIBLE_DRIVER:									return "VK_ERROR_INCOMPATIBLE_DRIVER";
			case VK_ERROR_TOO_MANY_OBJECTS:										return "VK_ERROR_TOO_MANY_OBJECTS";
			case VK_ERROR_FORMAT_NOT_SUPPORTED:									return "VK_ERROR_FORMAT_NOT_SUPPORTED";
			case VK_ERROR_FRAGMENTED_POOL:										return "VK_ERROR_FRAGMENTED_POOL";
			case VK_ERROR_UNKNOWN:												return "VK_ERROR_UNKNOWN";
			case VK_ERROR_OUT_OF_POOL_MEMORY:									return "VK_ERROR_OUT_OF_POOL_MEMORY";
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:								return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
			case VK_ERROR_FRAGMENTATION:										return "VK_ERROR_FRAGMENTATION";
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:						return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
			case VK_ERROR_SURFACE_LOST_KHR:										return "VK_ERROR_SURFACE_LOST_KHR";
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:								return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
			case VK_SUBOPTIMAL_KHR:												return "VK_SUBOPTIMAL_KHR";
			case VK_ERROR_OUT_OF_DATE_KHR:										return "VK_ERROR_OUT_OF_DATE_KHR";
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:								return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
			case VK_ERROR_VALIDATION_FAILED_EXT:								return "VK_ERROR_VALIDATION_FAILED_EXT";
			case VK_ERROR_INVALID_SHADER_NV:									return "VK_ERROR_INVALID_SHADER_NV";
			case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:			return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
			case VK_ERROR_NOT_PERMITTED_EXT:									return "VK_ERROR_NOT_PERMITTED_EXT";
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:					return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
			case VK_THREAD_IDLE_KHR:											return "VK_THREAD_IDLE_KHR";
			case VK_THREAD_DONE_KHR:											return "VK_THREAD_DONE_KHR";
			case VK_OPERATION_DEFERRED_KHR:										return "VK_OPERATION_DEFERRED_KHR";
			case VK_OPERATION_NOT_DEFERRED_KHR:									return "VK_OPERATION_NOT_DEFERRED_KHR";
			case VK_PIPELINE_COMPILE_REQUIRED_EXT:								return "VK_PIPELINE_COMPILE_REQUIRED_EXT";
			default:															return "???";
		}
	}

	static constexpr const char* description_short(const VkResult& result)
	{
		switch (result)
		{
			case VK_SUCCESS:													return "Success";
			case VK_NOT_READY:													return "Not ready";
			case VK_TIMEOUT:													return "Timeout";
			case VK_EVENT_SET:													return "Event set";
			case VK_EVENT_RESET:												return "Event reset";
			case VK_INCOMPLETE:													return "Incomplete";
			case VK_ERROR_OUT_OF_HOST_MEMORY:									return "Error: Out of host memory";
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:									return "Error: Out of device memory";
			case VK_ERROR_INITIALIZATION_FAILED:								return "Error: Initialization failed";
			case VK_ERROR_DEVICE_LOST:											return "Error: Device lost";
			case VK_ERROR_MEMORY_MAP_FAILED:									return "Error: Memory map failed";
			case VK_ERROR_LAYER_NOT_PRESENT:									return "Error: Layer not present";
			case VK_ERROR_EXTENSION_NOT_PRESENT:								return "Error: Extension not present";
			case VK_ERROR_FEATURE_NOT_PRESENT:									return "Error: Feature not present";
			case VK_ERROR_INCOMPATIBLE_DRIVER:									return "Error: Incompatible driver";
			case VK_ERROR_TOO_MANY_OBJECTS:										return "Error: Too many objects";
			case VK_ERROR_FORMAT_NOT_SUPPORTED:									return "Error: Format not supported";
			case VK_ERROR_FRAGMENTED_POOL:										return "Error: Fragmented pool";
			case VK_ERROR_UNKNOWN:												return "Error: Unknown";
			case VK_ERROR_OUT_OF_POOL_MEMORY:									return "Error: Out of pool memory";
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:								return "Error: Invalid external handle";
			case VK_ERROR_FRAGMENTATION:										return "Error: Fragmentation";
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:						return "Error: Invalid opaque capture address";
			case VK_ERROR_SURFACE_LOST_KHR:										return "Error: Surface lost (khr)";
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:								return "Error: Native window in use (khr)";
			case VK_SUBOPTIMAL_KHR:												return "Suboptimal (khr)";
			case VK_ERROR_OUT_OF_DATE_KHR:										return "Error: Out of date (khr)";
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:								return "Error: Incompatible display (khr)";
			case VK_ERROR_VALIDATION_FAILED_EXT:								return "Error: Validation failed (ext)";
			case VK_ERROR_INVALID_SHADER_NV:									return "Error: Invalid shader nv";
			case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:			return "Error: Invalid drm format modifier plane layout (ext)";
			case VK_ERROR_NOT_PERMITTED_EXT:									return "Error: Not permitted (ext)";
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:					return "Error: Full screen exclusive mode lost (ext)";
			case VK_THREAD_IDLE_KHR:											return "Thread idle (khr)";
			case VK_THREAD_DONE_KHR:											return "Thread done (khr)";
			case VK_OPERATION_DEFERRED_KHR:										return "Operation deferred (khr)";
			case VK_OPERATION_NOT_DEFERRED_KHR:									return "Operation not deferred (khr)";
			case VK_PIPELINE_COMPILE_REQUIRED_EXT:								return "Pipeline compile required (ext)";
			default:															return "???";
		}
	}

	static constexpr const char* description(const VkResult& result)
	{
		switch (result)
		{
			case VK_SUCCESS:													return "Command successfully completed.";
			case VK_NOT_READY:													return "A fence or query has not yet completed.";
			case VK_TIMEOUT:													return "A wait operation has not completed in the specified time.";
			case VK_EVENT_SET:													return "An event is signaled.";
			case VK_EVENT_RESET:												return "An event is unsignaled.";
			case VK_INCOMPLETE:													return "A return array was too small for the result.";
			case VK_SUBOPTIMAL_KHR:												return "A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.";
			case VK_THREAD_IDLE_KHR:											return "A deferred operation is not complete but there is currently no work for this thread to do at the time of this call.";
			case VK_THREAD_DONE_KHR:											return "A deferred operation is not complete but there is no work remaining to assign to additional threads.";
			case VK_OPERATION_DEFERRED_KHR:										return "A deferred operation was requested and at least some of the work was deferred.";
			case VK_OPERATION_NOT_DEFERRED_KHR:									return "A deferred operation was requested and no operations were deferred.";
			case VK_PIPELINE_COMPILE_REQUIRED_EXT:								return "A requested pipeline creation would have required compilation, but the application requested compilation to not be performed.";
			case VK_ERROR_OUT_OF_HOST_MEMORY:									return "A host memory allocation has failed.";
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:									return "A device memory allocation has failed.";
			case VK_ERROR_INITIALIZATION_FAILED:								return "Initialization of an object could not be completed for implementation-specific reasons.";
			case VK_ERROR_DEVICE_LOST:											return "The logical or physical device has been lost.";
			case VK_ERROR_MEMORY_MAP_FAILED:									return "Mapping of a memory object has failed.";
			case VK_ERROR_LAYER_NOT_PRESENT:									return "A requested layer is not present or could not be loaded.";
			case VK_ERROR_EXTENSION_NOT_PRESENT:								return "A requested extension is not supported.";
			case VK_ERROR_FEATURE_NOT_PRESENT:									return "A requested feature is not supported.";
			case VK_ERROR_INCOMPATIBLE_DRIVER:									return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons.";
			case VK_ERROR_TOO_MANY_OBJECTS:										return "Too many objects of the type have already been created.";
			case VK_ERROR_FORMAT_NOT_SUPPORTED:									return "A requested format is not supported on this device.";
			case VK_ERROR_FRAGMENTED_POOL:										return "A pool allocation has failed due to fragmentation of the pool’s memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation.";
			case VK_ERROR_SURFACE_LOST_KHR:										return "A surface is no longer available.";
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:								return "The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again.";
			case VK_ERROR_OUT_OF_DATE_KHR:										return "A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface.";
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:								return "The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image.";
			case VK_ERROR_INVALID_SHADER_NV:									return "One or more shaders failed to compile or link. More details are reported back to the application via VK_EXT_debug_report if enabled.";
			case VK_ERROR_OUT_OF_POOL_MEMORY:									return "A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead.";
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:								return "An external handle is not a valid handle of the specified type.";
			case VK_ERROR_FRAGMENTATION:										return "A descriptor pool creation has failed due to fragmentation.";
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:						return "A buffer creation or memory allocation failed because the requested address is not available. A shader group handle assignment failed because the requested shader group handle information is no longer valid.";
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:					return "An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application’s control.";
			case VK_ERROR_UNKNOWN:												return "An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred.";
			default:															return "???";
		}
	}

	static constexpr bool succeeded(const VkResult& result)
	{
#		ifdef RV_CHECK_STRICT
		return result == VK_SUCCESS;
#		else
		switch (result)
		{
			case VK_SUCCESS:						return true;
			case VK_NOT_READY:						return true;
			case VK_TIMEOUT:						return true;
			case VK_EVENT_SET:						return true;
			case VK_EVENT_RESET:					return true;
			case VK_INCOMPLETE:						return true;
			case VK_SUBOPTIMAL_KHR:					return true;
			case VK_THREAD_IDLE_KHR:				return true;
			case VK_THREAD_DONE_KHR:				return true;
			case VK_OPERATION_DEFERRED_KHR:			return true;
			case VK_OPERATION_NOT_DEFERRED_KHR:		return true;
			case VK_PIPELINE_COMPILE_REQUIRED_EXT:	return true;
			default:								return false;
		}
#		endif
	}
	static constexpr bool failed(const VkResult& result)
	{
#		ifdef RV_CHECK_STRICT
		return result != VK_SUCCESS;
#		else
		switch (result)
		{
			case VK_ERROR_OUT_OF_HOST_MEMORY:					return true;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:					return true;
			case VK_ERROR_INITIALIZATION_FAILED:				return true;
			case VK_ERROR_DEVICE_LOST:							return true;
			case VK_ERROR_MEMORY_MAP_FAILED:					return true;
			case VK_ERROR_LAYER_NOT_PRESENT:					return true;
			case VK_ERROR_EXTENSION_NOT_PRESENT:				return true;
			case VK_ERROR_FEATURE_NOT_PRESENT:					return true;
			case VK_ERROR_INCOMPATIBLE_DRIVER:					return true;
			case VK_ERROR_TOO_MANY_OBJECTS:						return true;
			case VK_ERROR_FORMAT_NOT_SUPPORTED:					return true;
			case VK_ERROR_FRAGMENTED_POOL:						return true;
			case VK_ERROR_SURFACE_LOST_KHR:						return true;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:				return true;
			case VK_ERROR_OUT_OF_DATE_KHR:						return true;
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:				return true;
			case VK_ERROR_INVALID_SHADER_NV:					return true;
			case VK_ERROR_OUT_OF_POOL_MEMORY:					return true;
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:				return true;
			case VK_ERROR_FRAGMENTATION:						return true;
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:		return true;
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:	return true;
			case VK_ERROR_UNKNOWN:								return true;
			default:											return false;
		}
#		endif
	}
}