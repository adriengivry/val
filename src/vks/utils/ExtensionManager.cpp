/**
* @project: vulkan-sandbox
* @author: Adrien Givry
* @licence: MIT
*/

#include <vks/utils/ExtensionManager.h>
#include <iostream>

namespace vks::utils
{
	ExtensionManager::ExtensionManager()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		m_extensions.resize(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, m_extensions.data());
	}

	bool ExtensionManager::IsExtensionSupported(const std::string_view extension) const
	{
		for (auto& currentExtension : m_extensions)
		{
			if (strcmp(currentExtension.extensionName, extension.data()) == 0)
			{
				return true;
			}
		}

		return false;
	}

	void ExtensionManager::LogExtensions() const
	{
		std::cout << m_extensions.size() << " available extensions:\n";

		for (const auto& extension : m_extensions)
		{
			std::cout << '\t' << extension.extensionName << '\n';
		}
	}

	std::vector<const char*> ExtensionManager::FilterExtensions(std::span<const RequestedExtension> p_requestedExtensions) const
	{
		std::vector<const char*> filteredExtensions;

		for (const auto& extension : p_requestedExtensions)
		{
			if (!IsExtensionSupported(extension.name))
			{
				if (extension.required)
				{
					throw std::runtime_error("required extension not supported!");
				}
				else
				{
					std::cout << "Optional requested extension '" << extension.name << "' isn't supported: skipped\n";
				}
			}
			else
			{
				filteredExtensions.push_back(extension.name.c_str());
			}
		}

		return filteredExtensions;
	}
}
