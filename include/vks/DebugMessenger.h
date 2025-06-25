/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <string>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>

namespace vks
{
	class DebugMessengerUtil
	{
	public:
		static VkDebugUtilsMessengerCreateInfoEXT GenerateCreateInfo();
	};

	class DebugMessenger
	{
	public:
		/**
		* Creates a debug messenger.
		*/
		DebugMessenger(VkInstance& p_instance, const VkDebugUtilsMessengerCreateInfoEXT& p_createInfo);

		/**
		* Destroys the debug messenger
		*/
		virtual ~DebugMessenger();

	private:
		VkInstance& m_instance;
		VkDebugUtilsMessengerEXT m_handle;
	};
}
