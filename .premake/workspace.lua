
workspace "LiquidEngine"
    basedir "../workspace/"
    language "C++"
    cppdialect "C++17"
    architecture "x86_64"
    
    setupLibraryDirectories{}
    setupPlatformDefines{}
    linkPlatformLibraries{}
    setupToolsetOptions{}
    
    includedirs {
        "../engine/src",
        "../platform-tools/include"
    }
    
    configurations { "Debug" }

    configuration "Debug"
        defines { "LIQUID_DEBUG" }
        symbols "On"

    configuration "Release"
        defines { "LIQUID_RELEASE" }
        optimize "On"
