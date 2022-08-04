
workspace "LiquidEngine"
    basedir "../workspace/"
    language "C++"
    cppdialect "C++17"
    architecture "x86_64"

    -- Set editor as starting project
    startproject "Liquidator"
    
    setupLibraryDirectories{}
    setupPlatformDefines{}
    linkPlatformLibraries{}
    setupToolsetOptions{}
    
    includedirs {
        "../engine/src",
        "../engine/rhi/core/include",
        "../engine/rhi/vulkan/include",
        "../engine/platform-tools/include"
    }
    
    configurations { "Debug", "Release", "Profile" }

    filter { "toolset:msc-*" }
        flags { "FatalCompileWarnings" }

    filter {"configurations:Debug"}
        defines { "LIQUID_DEBUG" }
        symbols "On"

    filter {"configurations:Release or configurations:Profile"}
        defines { "LIQUID_RELEASE" }
        optimize "On"

    filter {"configurations:Profile"}
        defines { "LIQUID_PROFILER" }
