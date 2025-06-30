workspace "val-examples"
	configurations { "Debug", "Release" }
	platforms { "x64" }
	startproject "sandbox"

outputdir = "%{wks.location}/../bin/"
objoutdir = "%{wks.location}/../obj/"
depsdir = "%{wks.location}/deps/"

include "sandbox"

group "deps"
	include "deps/_glm"
	include "deps/_glfw"
group ""

include "shaders"

include "../"