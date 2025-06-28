/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace vks::utils
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct SwapChainOptimalConfig
	{
		VkSurfaceCapabilitiesKHR capabilities;
		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode;
	};

	class SwapChainUtils
	{
	public:
		/**
		* Returns swap chain support details for a given physical device and a surface
		*/
		static SwapChainSupportDetails QuerySwapChainDetails(VkPhysicalDevice device, VkSurfaceKHR p_surface);

		/**
		* Create optimal swap chain config (input for swap chain creation)
		*/
		static SwapChainOptimalConfig CalculateSwapChainOptimalConfig(const SwapChainSupportDetails& p_details);

		/**
		* Calculate optimal swap chain images extents
		*/
		static VkExtent2D CalculateSwapExtent(VkSurfaceCapabilitiesKHR p_capabilities, VkExtent2D p_windowExtent);
	};
}
