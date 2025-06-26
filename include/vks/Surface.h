/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>

namespace vks
{
	struct SurfaceDesc
	{
		void* windowHandle; // is the Win32 HWND for the window to associate the surface with.
		void* instanceHandle; //  is the Win32 HINSTANCE for the window to associate the surface with.
	};

	class Surface
	{
	public:
		/**
		* Creates a surface
		*/
		Surface(VkInstance p_instance, const SurfaceDesc& p_desc);

		/**
		* Destroys the surface
		*/
		virtual ~Surface();

		/**
		* Returns the underlying VkSurfaceKHR handle
		*/
		VkSurfaceKHR GetHandle() const;

	private:
		VkInstance m_instance;
		VkSurfaceKHR m_surface;
	};
}
