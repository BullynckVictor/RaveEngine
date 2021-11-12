#pragma once
#include "Engine/Graphics/Instance.h"
#include <deque>
#include <string>
#include <vector>

namespace rv
{
	class DebugMessenger
	{
	public:
		DebugMessenger() = default;
		DebugMessenger(const DebugMessenger&) = delete;
		DebugMessenger(DebugMessenger&& rhs) noexcept;
		~DebugMessenger();

		DebugMessenger& operator= (const DebugMessenger&) = delete;
		DebugMessenger& operator= (DebugMessenger&& rhs) noexcept;

		static VkDebugUtilsMessengerCreateInfoEXT CreateInfo();

		void Release();
		static Result Create(DebugMessenger& messenger, const Instance& instance, VkDebugUtilsMessageSeverityFlagsEXT severity);
		static Result Create(
			DebugMessenger& messenger, 
			const Instance& instance, 
			Flags<Severity> severity = combine(RV_SEVERITY_INFO, RV_SEVERITY_WARNING, RV_SEVERITY_ERROR), 
			bool verbose = true
		);

	public:
		struct Message
		{
			Result MakeResult() const;
			bool Valid() const;

			VkDebugUtilsMessageSeverityFlagBitsEXT severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
			std::string message;
			const char* messageIDName = nullptr;
		};

		void Flush();
		static void StaticFlush();
		std::vector<Result> Results() const;
		const std::deque<Message>& Messages() const;
		Result Check() const;
		static Result StaticCheck();

	public:
		static VKAPI_ATTR VkBool32 VKAPI_CALL Callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		);

	private:
		void pushMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);
				
	public:
		VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
		const Instance* instance = nullptr;

	private:
		std::deque<Message> messages;
		static Message lastMessage;
	};

	struct VulkanDebugMessage : public ResultInfo
	{
		VulkanDebugMessage() = default;
		VulkanDebugMessage(const DebugMessenger::Message& message);

		DebugMessenger::Message message;
	};

	static constexpr ResultCode vulkan_debug_error = ResultCode("Vulkan Debug Error", RV_SEVERITY_ERROR);
	static constexpr ResultCode vulkan_debug_warning = ResultCode("Vulkan Debug Warning", RV_SEVERITY_WARNING);
	static constexpr ResultCode vulkan_debug_info = ResultCode("Vulkan Debug Info", RV_SEVERITY_INFO);

	static DebugMessenger debugMessenger;
}