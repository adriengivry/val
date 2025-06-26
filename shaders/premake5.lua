project "shaders"
	kind "Utility"

    targetdir "%{wks.location}/bin/%{cfg.buildcfg}/shaders"
    objdir "%{wks.location}/obj/%{cfg.buildcfg}/shaders"

	files {
		"**.vert",
        "**.frag",
        "**.comp",
        "**.geom",
        "**.tesc",
        "**.tese"
	}

    local shadersOutputDir = "%{wks.location}/assets/shaders/"
    
    -- Helper function to create shader compilation rules
    local function addShaderRule(extension, shaderType)
        filter("files:**." .. extension)
            buildmessage("Compiling " .. shaderType .. " shader %{file.name}")
            buildcommands {
                "if not exist \"" .. shadersOutputDir .. "\" mkdir \"" .. shadersOutputDir .. "\"",
                "\"%{VULKAN_SDK}\\bin\\glslangValidator.exe\" -V \"%{file.relpath}\" -o \"" .. shadersOutputDir .. "%{file.basename}." .. extension .. ".spv\""
            }
            buildoutputs { shadersOutputDir .. "%{file.basename}." .. extension .. ".spv" }
    end
    
    -- Add rules for all shader types
    addShaderRule("vert", "vertex")
    addShaderRule("frag", "fragment")
    addShaderRule("comp", "compute")
    addShaderRule("geom", "geometry")
    addShaderRule("tesc", "tessellation control")
    addShaderRule("tese", "tessellation evaluation")
    