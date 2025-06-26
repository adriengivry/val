outputdir = "%{wks.location}/bin/"
objoutdir = "%{wks.location}/obj/"
depsdir = "%{wks.location}/deps/"

VULKAN_SDK = os.getenv("VK_SDK_PATH")

-- Check if VULKAN_SDK is set to a valid path
if not VULKAN_SDK or VULKAN_SDK == "" then
    error("Couldn't find Vulkan SDK. Please make sure the Vulkan SDK is installed and the VK_SDK_PATH environment variable is set.")
end

print("Vulkan SDK Path: " .. VULKAN_SDK)

workspace "vulkan-sandbox"
	configurations { "Debug", "Release" }
	platforms { "x64" }
	startproject "vulkan-sandbox"

group "deps"
    include "deps/_glm"
    include "deps/_glfw"
group ""

    include "shaders/"

project "vulkan-sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	debugdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")

    files { "include/**.h", "src/**.cpp" }

    includedirs {
		"%{VULKAN_SDK}/include",
        "deps/_glfw/glfw/include",
        "deps/_glm/glm",
        "include"
    }

    links {
		"%{VULKAN_SDK}/lib/vulkan-1.lib",
        "glfw",
        "glm",
        "shaders"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    -- Shader compilation
    filter {}
