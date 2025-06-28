/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <vks/Device.h>
#include <vks/CommandBuffer.h>

namespace vks
{
	class CommandPool
	{
	public:
		/**
		* Creates a command pool
		*/
		CommandPool(Device& p_device);

		/**
		* Destroys the command pool
		*/
		virtual ~CommandPool();

		/**
		* Allocates a command buffer from the command pool
		*/
		std::vector<std::reference_wrapper<CommandBuffer>> AllocateCommandBuffers(uint32_t p_count, VkCommandBufferLevel p_level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		/**
		* Returns the command pool handle
		*/
		VkCommandPool GetHandle() const;

	private:
		Device& m_device;
		VkCommandPool m_handle = VK_NULL_HANDLE;
		std::vector<CommandBuffer> m_commandBuffers;
	};
}
