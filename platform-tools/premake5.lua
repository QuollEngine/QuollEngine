project "LiquidEnginePlatformTools"
    basedir "../workspace/platform-tools"
    kind "StaticLib"
    configurations { 
        "Debug"
    }

    includedirs {
        "platform-tools/include"
    }

    files {
        "src/*.cpp", 
        "include/platform-tools/*.h", 
    }

    filter { "system:linux" }
    removefiles { "src/**.win32.cpp" }

    filter { "system:windows" }
        removefiles { "src/**.linux.cpp" }

    filter { "system:macosx" }
        files {
            "src/**.mm"
        }

    removefiles { "src/**.linux.cpp", "src/**.win32.cpp" }
