
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
        "../platform-tools/include"
    }
    
    configurations { "Debug", "Release", "Profile-Debug", "Profile-Release" }

    filter { "toolset:msc-*" }
        flags { "FatalCompileWarnings" }

    filter {"configurations:Debug or configurations:Profile-Debug"}
        defines { "LIQUID_DEBUG" }
        symbols "On"

    filter {"configurations:Release or configurations:Profile-Release"}
        defines { "LIQUID_RELEASE" }
        optimize "On"

    filter {"configurations:Profile-Debug or configurations:Profile-Release"}
        defines { "LIQUID_PROFILER" }
