VULKAN_SDK = os.getenv("VK_SDK_PATH")

-- Check if VULKAN_SDK is set to a valid path
if not VULKAN_SDK or VULKAN_SDK == "" then
    error("Couldn't find Vulkan SDK. Please make sure the Vulkan SDK is installed and the VK_SDK_PATH environment variable is set.")
end

print("Vulkan SDK Path: " .. VULKAN_SDK)

project "val"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir ("bin/%{cfg.buildcfg}/%{prj.name}")
	objdir ("obj/%{cfg.buildcfg}/%{prj.name}")

    files { "include/**.h", "src/**.cpp" }

    includedirs {
		"%{VULKAN_SDK}/include",
        "include"
    }

    links {
		"%{VULKAN_SDK}/lib/vulkan-1.lib",
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
