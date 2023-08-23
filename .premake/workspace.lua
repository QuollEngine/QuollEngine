
workspace "LiquidEngine"
    basedir "../workspace/"
    language "C++"
    cppdialect "C++20"
    architecture "x86_64"

    -- Set editor as starting project
    startproject "LiquidEditor"
    
    setupLibraryDirectories{}
    setupPlatformDefines{}
    linkPlatformLibraries{}
    setupToolsetOptions{}
    setupTestingOptions{}
    
    includedirs {
        "../engine/src",
        "../engine/rhi/core/include",
        "../platform/base/include"
    }
    
    configurations { "Debug", "Release", "Profile", "Test" }

    
    filter { "toolset:msc-*" }
       defines { "_SILENCE_CXX20_CISO646_REMOVED_WARNING" }
       flags { "FatalCompileWarnings" }

    filter {"configurations:Debug or configurations:Test"}
        defines { "LIQUID_DEBUG" }
        symbols "On"

    filter {"configurations:Release or configurations:Profile"}
        defines { "LIQUID_RELEASE" }
        optimize "On"

    filter {"configurations:Profile"}
        defines { "LIQUID_PROFILER" }

    defines { "configurations:Test" }
        defines { "LIQUID_TEST" }
