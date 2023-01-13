project "Liquidator"
    basedir "../workspace/editor"
    kind "ConsoleApp"

    includedirs {
        "./src"
    }

    loadSourceFiles{}
    linkDependenciesWith{"LiquidEngine", "LiquidEngineRHIVulkan", "LiquidEngineRHICore", "LiquidEnginePlatformTools", "vendor-libimguizmo", "meshoptimizer"}
    
    copyRuntime();

    postbuildcommands {
        "{MKDIR} %{cfg.buildtarget.directory}/assets/shaders/",
        "glslc ../../editor/assets/shaders/editor-grid.vert -o %{cfg.buildtarget.directory}/assets/shaders/editor-grid.vert.spv",
        "glslc ../../editor/assets/shaders/editor-grid.frag -o %{cfg.buildtarget.directory}/assets/shaders/editor-grid.frag.spv",
        "glslc ../../editor/assets/shaders/skeleton-lines.vert -o %{cfg.buildtarget.directory}/assets/shaders/skeleton-lines.vert.spv",
        "glslc ../../editor/assets/shaders/skeleton-lines.frag -o %{cfg.buildtarget.directory}/assets/shaders/skeleton-lines.frag.spv",
        "glslc ../../editor/assets/shaders/object-icons.vert -o %{cfg.buildtarget.directory}/assets/shaders/object-icons.vert.spv",
        "glslc ../../editor/assets/shaders/object-icons.frag -o %{cfg.buildtarget.directory}/assets/shaders/object-icons.frag.spv",
        "glslc ../../editor/assets/shaders/mouse-picking.vert -o %{cfg.buildtarget.directory}/assets/shaders/mouse-picking.vert.spv",
        "glslc ../../editor/assets/shaders/mouse-picking-skinned.vert -o %{cfg.buildtarget.directory}/assets/shaders/mouse-picking-skinned.vert.spv",
        "glslc ../../editor/assets/shaders/mouse-picking.frag -o %{cfg.buildtarget.directory}/assets/shaders/mouse-picking.frag.spv",
        "glslc ../../editor/assets/shaders/collidable-shape.vert -o %{cfg.buildtarget.directory}/assets/shaders/collidable-shape.vert.spv",
        "glslc ../../editor/assets/shaders/collidable-shape.frag -o %{cfg.buildtarget.directory}/assets/shaders/collidable-shape.frag.spv",
        "{MKDIR} %{cfg.buildtarget.directory}/assets/icons",
        "{MKDIR} %{cfg.buildtarget.directory}/assets/fonts",
        "{COPYDIR} ../../editor/assets/icons %{cfg.buildtarget.directory}/assets/icons",
        "{COPYDIR} ../../editor/assets/fonts %{cfg.buildtarget.directory}/assets/fonts"
    }

    copyEngineAssets()

project "LiquidatorTests"
    basedir "../workspace/editor-test"
    kind "ConsoleApp"

    configurations {
        "Debug"
    }

    includedirs {
        "../editor/tests",
        "../editor/src",
        "../engine/src"
    }

    filter{}

    files {
        "src/**.cpp",
        "tests/**.cpp",
        "tests/**.h"
    }

    removefiles {
        "src/liquid/rhi/vulkan/VmaImpl.cpp",
        "src/main.cpp"
    }

    setupTestingOptions{}
    links { "LiquidEngine", "LiquidEngineRHICore", "LiquidEngineRHIVulkan", "LiquidEnginePlatformTools", "vendor-libimguizmo" }
    linkGoogleTest{}
    linkDependenciesWithoutVulkan{}

    copyEngineAssets()

    postbuildcommands {
        "{MKDIR} %{cfg.buildtarget.directory}/fixtures",
        "{COPYDIR} ../../editor/tests/fixtures %{cfg.buildtarget.directory}/fixtures"
    }

