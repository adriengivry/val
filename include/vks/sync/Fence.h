/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>

namespace vks::sync
{
	class Fence
	{
	public:
		/**
		* Creates a semaphore
		*/
		Fence(VkDevice p_device, bool p_createSignaled = false);

		/**
		* Destroys the semaphore
		*/
		virtual ~Fence();

		/**
		* Returns the underlying VkFence handle
		*/
		VkFence GetHandle() const;

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkFence m_handle = VK_NULL_HANDLE;
	};
}
