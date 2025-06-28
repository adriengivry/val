/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>

namespace vks
{
	class CommandPool;

	class CommandBuffer
	{
	public:
		/**
		* Destroys the command buffer
		*/
		virtual ~CommandBuffer() = default;

		/**
		* Returns the command buffer handle
		*/
		VkCommandBuffer GetHandle() const;

		/**
		* Resets the command buffer
		*/
		void Reset();

		/**
		* Begin recording commands
		*/
		void Begin();

		/**
		* Finish recording commands
		*/
		void End();

	private:
		CommandBuffer(VkCommandBuffer p_handle);

		friend class CommandPool;

	private:
		VkCommandBuffer m_handle = VK_NULL_HANDLE;
	};
}
