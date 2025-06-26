/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#if defined(_WIN32) || defined(_WIN64)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vks/Surface.h>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace vks
{
	Surface::Surface(VkInstance p_instance, const SurfaceDesc& p_desc) :
		m_instance(p_instance)
	{
		assert((p_desc.windowHandle && p_desc.instanceHandle) && "incomplete surface desc");
		
#if defined(_WIN32) || defined(_WIN64)
		VkWin32SurfaceCreateInfoKHR createInfo{
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.hinstance = static_cast<HINSTANCE>(p_desc.instanceHandle),
			.hwnd = static_cast<HWND>(p_desc.windowHandle)
		};

		if (vkCreateWin32SurfaceKHR(
			m_instance,
			&createInfo,
			nullptr,
			&m_surface
		) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
#else
		throw std::runtime_error("other platforms than windows aren't supported yet!");
#endif
	}

	Surface::~Surface()
	{
		vkDestroySurfaceKHR(
			m_instance,
			m_surface,
			nullptr
		);
	}

	VkSurfaceKHR Surface::GetHandle() const
	{
		assert(m_surface != VK_NULL_HANDLE);
		return m_surface;
	}
}
