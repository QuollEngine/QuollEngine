-- Link dependencies shared
-- by all projects
function linkDependencies()
    links { "vendor-libimgui", "glfw3", "ktx" }

    filter { "system:windows" }
        links { "vulkan-1" }

    filter { "system:linux or system:macosx" }
        links { "vulkan" }

    filter { "system:linux" }
        links { "Xrandr", "Xi", "X11", "dl" }

    filter{}
end

workspace "LiquidEngine"
    basedir "workspace/"
    configurations { "Debug" }
    language "C++"
    cppdialect "C++17"
    architecture "x86_64"

    sysincludedirs {
        "vendor/include"
    }

    includedirs {
        "engine/src"
    }

    libdirs {
        "vendor/lib"
    }

    -- This define is needed for libktx to reference
    -- static library functions instead of DLL
    defines {
        "KHRONOS_STATIC"
    }

    -- Vulkan SDK for Windows adds environment
    -- variable that points to Vulkan SDK
    -- installation
    filter { "system:windows" }
        sysincludedirs {
            "$(VULKAN_SDK)/Include"
        }

        libdirs {
            "$(VULKAN_SDK)/Lib"
        }

        defines {
            "LIQUID_PLATFORM_WINDOWS"
        }

    -- Vulkan SDK for macOS is installed to
    -- global include, library, and bin dirs
    filter { "system:macosx" }
        sysincludedirs {
            "/usr/local/include"
        }

        libdirs {
            "/usr/local/lib"
        }

        defines {
            "LIQUID_PLATFORM_MACOS"
        }

        xcodebuildsettings {
            ["MACOSX_DEPLOYMENT_TARGET"] = "11.1",
            ["ONLY_ACTIVE_ARCH"] = "YES",
            ["ENABLE_TESTABILITY"] = "YES",
            ["ARCHS"] = "$(ARCHS_STANDARD)",
            ["CLANG_ENABLE_OBJC_WEAK"] = "YES",
            ["CLANG_WARN_BLOCK_CAPTURE_AUTORELEASING"] = "YES",
            ["CLANG_WARN_BOOL_CONVERSION"] = "YES",
            ["CLANG_WARN_COMMA"] = "YES",
            ["CLANG_WARN_CONSTANT_CONVERSION"] = "YES",
            ["CLANG_WARN_DEPRECATED_OBJC_IMPLEMENTATIONS"] = "YES",
            ["CLANG_WARN_EMPTY_BODY"] = "YES",
            ["CLANG_WARN_ENUM_CONVERSION"] = "YES",
            ["CLANG_WARN_INFINITE_RECURSION"] = "YES",
            ["CLANG_WARN_INT_CONVERSION"] = "YES",
            ["CLANG_WARN_NON_LITERAL_NULL_CONVERSION"] = "YES",
            ["CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF"] = "YES",
            ["CLANG_WARN_OBJC_LITERAL_CONVERSION"] = "YES",
            ["CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER"] = "YES",
            ["CLANG_WARN_RANGE_LOOP_ANALYSIS"] = "YES",
            ["CLANG_WARN_STRICT_PROTOTYPES"] = "YES",
            ["CLANG_WARN_SUSPICIOUS_MOVE"] = "YES",
            ["CLANG_WARN_UNREACHABLE_CODE"] = "YES",
            ["CLANG_WARN__DUPLICATE_METHOD_MATCH"] = "YES",
            ["GCC_WARN_64_TO_32_BIT_CONVERSION"] = "YES",
            ["GCC_WARN_ABOUT_RETURN_TYPE"] = "YES",
            ["GCC_WARN_UNDECLARED_SELECTOR"] = "YES",
            ["GCC_WARN_UNINITIALIZED_AUTOS"] = "YES",
            ["GCC_WARN_UNUSED_FUNCTION"] = "YES",
            ["GCC_WARN_UNUSED_VARIABLE"] = "YES",
            ["GCC_NO_COMMON_BLOCKS"] = "YES",
            ["ENABLE_STRICT_OBJC_MSGSEND"] = "YES",
        }

        -- GLFW is linked statically; so, we need to 
        -- link the necessary frameworks
        links {
            "Cocoa.framework",
            "CoreFoundation.framework",
            "IOKit.framework",
            "CoreVideo.framework"
        }

        filter { "configurations:Debug" }
            xcodebuildsettings {
                ["CLANG_ENABLE_OBJC_WEAK"] = "YES",
                ["CODE_SIGN_IDENTITY"] = "-"
            }

    -- TODO: Support linux window
    filter { "system:linux" }
        linkoptions { "-pthread" }

        defines {
            "LIQUID_PLATFORM_LINUX"
        }

    -- Sets working directory for Visual Studio
    -- so that, applications use the same path
    -- as if we were to open exe file directly
    filter { "toolset:msc-*" }
        debugdir "$(TargetDir)"

    configuration "Debug"
        defines { "LIQUID_DEBUG" }
        symbols "On"

    configuration "Release"
        defines { "LIQUID_RELEASE" }
        optimize "On"

project "vendor-libimgui"
    basedir "vendor/projects/imgui"
    kind "StaticLib"

    files {
        "vendor/projects/imgui/*.cpp",
    }

project "LiquidEngine"
    basedir "workspace/engine"
    kind "StaticLib"

    pchheader "../../engine/src/core/Base.h"

    filter { "toolset:msc-*" }
        pchheader "core/Base.h"
        pchsource "engine/src/core/Base.cpp"

    filter{}

    files {
        "engine/src/**.h",
        "engine/src/**.cpp"
    }

    linkDependencies{}

    postbuildcommands {
        "{MKDIR} %{cfg.buildtarget.directory}/assets/shaders/",
        "glslc ../../engine/assets/shaders/pbr.vert -o %{cfg.buildtarget.directory}/assets/shaders/pbr.vert.spv",
        "glslc ../../engine/assets/shaders/pbr.frag -o %{cfg.buildtarget.directory}/assets/shaders/pbr.frag.spv",
        "glslc ../../engine/assets/shaders/skybox.frag -o %{cfg.buildtarget.directory}/assets/shaders/skybox.frag.spv",
        "glslc ../../engine/assets/shaders/skybox.vert -o %{cfg.buildtarget.directory}/assets/shaders/skybox.vert.spv",
        "glslc ../../engine/assets/shaders/shadowmap.frag -o %{cfg.buildtarget.directory}/assets/shaders/shadowmap.frag.spv",
        "glslc ../../engine/assets/shaders/shadowmap.vert -o %{cfg.buildtarget.directory}/assets/shaders/shadowmap.vert.spv",
        "glslc ../../engine/assets/shaders/imgui.frag -o %{cfg.buildtarget.directory}/assets/shaders/imgui.frag.spv",
        "glslc ../../engine/assets/shaders/imgui.vert -o %{cfg.buildtarget.directory}/assets/shaders/imgui.vert.spv"
    }

project "LiquidEngineTest"
    basedir "workspace/engine-test/"
    kind "ConsoleApp"

    files {
        "engine/src/**.cpp",
        "engine/src/**.h",
        "engine/tests/**.cpp",
        "engine/tests/**.h"
    }

    removefiles {
        "engine/src/renderer/vulkan/VmaImpl.cpp",
    }

    postbuildcommands {
        "{COPYFILE} ../../engine/tests/fixtures/white-image-100x100.png %{cfg.buildtarget.directory}/white-image-100x100.png",
        "{COPYFILE} ../../engine/tests/fixtures/1x1-cubemap.ktx %{cfg.buildtarget.directory}/1x1-cubemap.ktx",
        "{COPYFILE} ../../engine/tests/fixtures/1x1-2d.ktx %{cfg.buildtarget.directory}/1x1-2d.ktx",
        "{COPYFILE} ../../engine/tests/fixtures/1x1-1d.ktx %{cfg.buildtarget.directory}/1x1-1d.ktx"
    }

    links { "vendor-libimgui", "glfw3", "ktx" }

    filter { "system:windows" }
        links { "gtestd", "gtest_maind", "gmockd" }

    filter { "system:macosx or system:linux" }
        links { "gtest", "gtest_main", "gmock", "Xrandr", "Xi", "X11", "dl" }

    filter { "toolset:clang" }
        buildoptions {
            "-fprofile-instr-generate",
            "-fcoverage-mapping"
        }

        linkoptions {
            "-fprofile-instr-generate",
            "-fcoverage-mapping"
        }

    filter { "toolset:gcc" }
        buildoptions {
            "-fprofile-arcs",
            "-ftest-coverage"
        }

        linkoptions {
            "-fprofile-arcs",
            "-ftest-coverage"
        }

project "DemoBasicTriangle"
    basedir "workspace/demos/basic-triangle"
    kind "ConsoleApp"
    configurations { "Debug" }

    files {
        "demos/basic-triangle/src/**.h",
        "demos/basic-triangle/src/**.cpp"
    }

    links { "LiquidEngine" }
    linkDependencies{}

    postbuildcommands {
        "glslc ../../../demos/basic-triangle/assets/basic-shader.vert -o %{cfg.buildtarget.directory}/basic-shader.vert.spv",
        "glslc ../../../demos/basic-triangle/assets/basic-shader.frag -o %{cfg.buildtarget.directory}/basic-shader.frag.spv",
        "glslc ../../../demos/basic-triangle/assets/red-shader.frag -o %{cfg.buildtarget.directory}/red-shader.frag.spv",
        "glslc ../../../demos/basic-triangle/assets/texture-shader.frag -o %{cfg.buildtarget.directory}/texture-shader.frag.spv",
        "glslc ../../../demos/basic-triangle/assets/texture-shader.vert -o %{cfg.buildtarget.directory}/texture-shader.vert.spv",
        "{COPYFILE} ../../../demos/basic-triangle/assets/textures/brick.png %{cfg.buildtarget.directory}/brick.png"
    }

    filter { "system:linux" }
        links {
            "Xrandr",
            "Xi",
            "X11",
        }

project "DemoPong"
    basedir "workspace/demos/pong-3d"
    kind "ConsoleApp"
    configurations { "Debug" }
    files {
        "demos/pong-3d/src/**.h",
        "demos/pong-3d/src/**.cpp"
    }

    links { "LiquidEngine" }
    linkDependencies{}

    postbuildcommands {
        "glslc ../../../demos/pong-3d/assets/basic-shader.vert -o %{cfg.buildtarget.directory}/basic-shader.vert.spv",
        "glslc ../../../demos/pong-3d/assets/basic-shader.frag -o %{cfg.buildtarget.directory}/basic-shader.frag.spv"
    }

    filter { "system:linux" }
        links {
            "Xrandr",
            "Xi",
            "X11",
        }

project "DemoSceneViewer"
    basedir "workspace/demos/scene-viewer"
    kind "ConsoleApp"
    configurations { "Debug" }
    files {
        "demos/scene-viewer/src/**.h",
        "demos/scene-viewer/src/**.cpp"
    }

    links { "LiquidEngine" }
    linkDependencies{}

    filter { "system:linux" }
        removefiles { "demos/scene-viewer/src/**.win32.cpp" }
        links {
            "Xrandr",
            "Xi",
            "X11",
        }
    
    filter { "system:windows" }
        removefiles { "demos/scene-viewer/src/**.linux.cpp" }

    filter { "system:macosx" }
        files {
            "demos/scene-viewer/src/**.mm"
        }

        removefiles { "demos/scene-viewer/src/**.linux.cpp", "demos/scene-viewer/src/**.win32.cpp" }

        links { "AppKit.framework" }

    
