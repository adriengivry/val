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

namespace vks::utils
{
	struct RequestedExtension
	{
		std::string name;
		bool required;
	};

	class ExtensionManager
	{
	public:
		/**
		* Creates the extension manager, and automatically fetches supported extensions
		*/
		ExtensionManager();

		/**
		* Destroys the extension manager
		*/
		virtual ~ExtensionManager() = default;

		/**
		* Returns true if a given extension is supported
		*/
		bool IsExtensionSupported(const std::string_view extension) const;

		/**
		* Prints the given list of extensions to the console
		*/
		void LogExtensions() const;

		/**
		* Returns a formatted list of available extensions, skipping any unsupported extension,
		* and returning an error if a required extension isn't available
		*/
		std::vector<const char*> FilterExtensions(std::span<const RequestedExtension> p_requestedExtensions) const;

	private:
		std::vector<VkExtensionProperties> m_extensions;
	};
}
