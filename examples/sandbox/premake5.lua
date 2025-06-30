project "sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	debugdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")

    files { "include/**.h", "src/**.cpp" }

    includedirs {
		"%{VULKAN_SDK}/include",
        "../deps/_glfw/glfw/include",
        "../deps/_glm/glm",
        "../../include",
        "include"
    }

    links {
		"%{VULKAN_SDK}/lib/vulkan-1.lib",
        "glfw",
        "glm",
        "val",
        "shaders"
    }

    -- Copy assets folder to output directory
    buildaction "Custom"
    buildmessage "Copying assets to output folder..."
    buildcommands {
        "{COPYDIR} %{wks.location}assets %{cfg.targetdir}/assets"
    }
    buildoutputs { "%{cfg.targetdir}/assets_copied.stamp" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"