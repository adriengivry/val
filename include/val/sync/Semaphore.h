/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>

namespace val::sync
{
	class Semaphore
	{
	public:
		/**
		* Creates a semaphore
		*/
		Semaphore(VkDevice p_device);

		/**
		* Destroys the semaphore
		*/
		virtual ~Semaphore();

		/**
		* Returns the underlying VkSemaphore handle
		*/
		VkSemaphore GetHandle() const;

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkSemaphore m_handle = VK_NULL_HANDLE;
	};
}
