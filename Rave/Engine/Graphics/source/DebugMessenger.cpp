#include "Engine/Graphics/DebugMessenger.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/String.h"
#include "Engine/Core/Logger.h"
#ifndef RV_DEBUG_LOGGER
#include "Engine/Core/SystemInclude.h"
#include <iostream>
#endif
#include <algorithm>

rv::DebugMessenger::Message rv::DebugMessenger::lastMessage{};

template<>
void rv::destroy(VkDebugUtilsMessengerEXT messenger, VkDevice device, VkInstance instance)
{
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (vkDestroyDebugUtilsMessengerEXT)
		vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
}

rv::DebugMessenger::DebugMessenger(DebugMessenger&& rhs) noexcept
	:
	messenger(move(rhs.messenger)),
	instance(move(rhs.instance)),
	messages(std::move(rhs.messages))
{
}

rv::DebugMessenger::~DebugMessenger()
{
	Release();
}

rv::DebugMessenger& rv::DebugMessenger::operator=(DebugMessenger&& rhs) noexcept
{
	messenger = move(rhs.messenger);
	messages = std::move(rhs.messages);
	instance = move(rhs.instance);
	return *this;
}

VkDebugUtilsMessengerCreateInfoEXT rv::DebugMessenger::CreateInfo()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = Callback;
	return createInfo;
}

void rv::DebugMessenger::Release()
{
	if (instance)
		release(messenger, *instance);
	else
		release(messenger);

	Result result = Check();
	if (result.failed())
		lastMessage = dynamic_cast<const VulkanDebugMessage&>(result.info()).message;

	messages.clear();
	instance = nullptr;
}

rv::Result rv::DebugMessenger::Create(DebugMessenger& messenger, const Instance& instance, VkDebugUtilsMessageSeverityFlagsEXT severity)
{
	messenger.Release();

	messenger.instance = &instance;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = CreateInfo();
	createInfo.messageSeverity = severity;
	createInfo.pUserData = &messenger;

	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = instance.GetProc<PFN_vkCreateDebugUtilsMessengerEXT>("vkCreateDebugUtilsMessengerEXT");
	if (vkCreateDebugUtilsMessengerEXT)
		return rv_try_vkr_info(vkCreateDebugUtilsMessengerEXT(instance.instance, &createInfo, nullptr, &messenger.messenger), "Unable to create DebugMessenger");
	else
		return rv_try_vkr_info(VK_ERROR_EXTENSION_NOT_PRESENT, "Unable to create DebugMessenger");
}

rv::Result rv::DebugMessenger::Create(DebugMessenger& messenger, const Instance& instance, Flags<Severity> severity, bool verbose)
{
	VkDebugUtilsMessageSeverityFlagsEXT flags = verbose ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT : 0;
	flags |= severity.contain(RV_SEVERITY_INFO) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT : 0;
	flags |= severity.contain(RV_SEVERITY_WARNING) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT : 0;
	flags |= severity.contain(RV_SEVERITY_ERROR) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT : 0;
	return Create(messenger, instance, flags);
}

void rv::DebugMessenger::Flush()
{
	messages.clear();
	StaticFlush();
}

void rv::DebugMessenger::StaticFlush()
{
	lastMessage.message.clear();
	lastMessage.messageIDName.clear();
	lastMessage.severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
}

std::vector<rv::Result> rv::DebugMessenger::Results() const
{
	std::vector<Result> results(messages.size() + 1);
	std::transform(messages.begin(), messages.end(), results.begin(), [](const Message& message) {
		return message.MakeResult();
	});
	results.back() = lastMessage.MakeResult();
	return results;
}

const std::deque<rv::DebugMessenger::Message>& rv::DebugMessenger::Messages() const
{
	return messages;
}

rv::Result rv::DebugMessenger::Check() const
{
	VkDebugUtilsMessageSeverityFlagsEXT max_severity = 0;
	const Message* m = nullptr;

	for (const auto& message : messages)
	{
		if ((VkDebugUtilsMessageSeverityFlagsEXT)message.severity > max_severity)
		{
			max_severity = (VkDebugUtilsMessageSeverityFlagsEXT)message.severity;
			m = &message;
		}
	}
	if (lastMessage.Valid() && (VkDebugUtilsMessageSeverityFlagsEXT)lastMessage.severity > max_severity)
	{
		max_severity = (VkDebugUtilsMessageSeverityFlagsEXT)lastMessage.severity;
		m = &lastMessage;
	}
	if (m)
		return m->MakeResult();
	return vulkan_debug_info;
}

bool rv::DebugMessenger::Message::Valid() const
{
	return !messageIDName.empty();
}

rv::Result rv::DebugMessenger::StaticCheck()
{
	if (lastMessage.Valid())
		return lastMessage.MakeResult();
	return vulkan_debug_info;
}

VKAPI_ATTR VkBool32 VKAPI_CALL rv::DebugMessenger::Callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	static bool thrown = false;
	if (pUserData)
	{
		DebugMessenger& messenger = *reinterpret_cast<DebugMessenger*>(pUserData);
		messenger.pushMessage(messageSeverity, pCallbackData);
	}
	else
	{
		lastMessage = Message{ messageSeverity, pCallbackData->pMessage, pCallbackData->pMessageIdName };
	}
	Severity severity;
	switch (messageSeverity)
	{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			severity = RV_SEVERITY_ERROR;
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			severity = RV_SEVERITY_WARNING;
			break;

		default:
			severity = RV_SEVERITY_INFO;
			break;
	}

#ifdef RV_DEBUG_LOGGER
	debug.Log(pCallbackData->pMessage, severity);
#else
	rv::Message message = rv::Message(pCallbackData->pMessage, severity);
	message.message.push_back('\n');
	std::cout << message.message;
	if constexpr (cti.platform.windows)
		OutputDebugString(message.message.c_str());
#endif

	if (!thrown && messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		thrown = false;
		Result(vulkan_debug_error, new VulkanDebugMessage(Message{ messageSeverity, pCallbackData->pMessage, pCallbackData->pMessageIdName })).throw_exception();
	}
	return VK_FALSE;
}

void rv::DebugMessenger::pushMessage(VkDebugUtilsMessageSeverityFlagBitsEXT severity, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
{
	messages.emplace_back(severity, pCallbackData->pMessage, pCallbackData->pMessageIdName);
}

rv::Result rv::DebugMessenger::Message::MakeResult() const
{
	ResultCode code;

	switch (severity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		code = vulkan_debug_error;
		break;

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		code = vulkan_debug_warning;
		break;

	default:
		code = vulkan_debug_info;
		break;
	}
		
	return Result(code, new VulkanDebugMessage(*this));
}

rv::VulkanDebugMessage::VulkanDebugMessage(const DebugMessenger::Message& message)
	:
	ResultInfo(message.message, { str("ID: ", message.messageIDName) }),
	message(message)
{
}