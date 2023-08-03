project "LiquidEditor"
    basedir "../workspace/editor"
    kind "ConsoleApp"
    targetname "Liquidator"

    includedirs {
        "./src",
    }

    loadSourceFiles{}
    linkOptick{}
    linkDependenciesWith{"LiquidEngine", "vendor-libimguizmo", "vendor-libmikktspace", "meshoptimizer"}
    linkVulkanRHI{}
    linkPlatform()
    
    copyRuntime();

    postbuildcommands {
        "{MKDIR} %{cfg.buildtarget.directory}/assets/shaders/",
        "glslc ../../editor/assets/shaders/editor-grid.vert -o %{cfg.buildtarget.directory}/assets/shaders/editor-grid.vert.spv",
        "glslc ../../editor/assets/shaders/editor-grid.frag -o %{cfg.buildtarget.directory}/assets/shaders/editor-grid.frag.spv",
        "glslc ../../editor/assets/shaders/skeleton-lines.vert -o %{cfg.buildtarget.directory}/assets/shaders/skeleton-lines.vert.spv",
        "glslc ../../editor/assets/shaders/skeleton-lines.frag -o %{cfg.buildtarget.directory}/assets/shaders/skeleton-lines.frag.spv",
        "glslc ../../editor/assets/shaders/object-icons.vert -o %{cfg.buildtarget.directory}/assets/shaders/object-icons.vert.spv",
        "glslc ../../editor/assets/shaders/object-icons.frag -o %{cfg.buildtarget.directory}/assets/shaders/object-icons.frag.spv",
        "glslc ../../editor/assets/shaders/mouse-picking-sprite.vert -o %{cfg.buildtarget.directory}/assets/shaders/mouse-picking-sprite.vert.spv",
        "glslc ../../editor/assets/shaders/mouse-picking-mesh.vert -o %{cfg.buildtarget.directory}/assets/shaders/mouse-picking-mesh.vert.spv",
        "glslc ../../editor/assets/shaders/mouse-picking-skinned-mesh.vert -o %{cfg.buildtarget.directory}/assets/shaders/mouse-picking-skinned-mesh.vert.spv",
        "glslc ../../editor/assets/shaders/mouse-picking-text.vert -o %{cfg.buildtarget.directory}/assets/shaders/mouse-picking-text.vert.spv",
        "glslc ../../editor/assets/shaders/mouse-picking-selector.frag -o %{cfg.buildtarget.directory}/assets/shaders/mouse-picking-selector.frag.spv",
        "glslc ../../editor/assets/shaders/collidable-shape.vert -o %{cfg.buildtarget.directory}/assets/shaders/collidable-shape.vert.spv",
        "glslc ../../editor/assets/shaders/collidable-shape.frag -o %{cfg.buildtarget.directory}/assets/shaders/collidable-shape.frag.spv",
        "glslc ../../editor/assets/shaders/outline-sprite.vert -o %{cfg.buildtarget.directory}/assets/shaders/outline-sprite.vert.spv",
        "glslc ../../editor/assets/shaders/outline-geometry.vert -o %{cfg.buildtarget.directory}/assets/shaders/outline-geometry.vert.spv",
        "glslc ../../editor/assets/shaders/outline-skinned-geometry.vert -o %{cfg.buildtarget.directory}/assets/shaders/outline-skinned-geometry.vert.spv",
        "glslc ../../editor/assets/shaders/outline-color.frag -o %{cfg.buildtarget.directory}/assets/shaders/outline-color.frag.spv",
        "glslc ../../editor/assets/shaders/equirectangular-to-cubemap.comp -o %{cfg.buildtarget.directory}/assets/shaders/equirectangular-to-cubemap.comp.spv",
        "glslc ../../editor/assets/shaders/generate-irradiance-map.comp -o %{cfg.buildtarget.directory}/assets/shaders/generate-irradiance-map.comp.spv",
        "glslc ../../editor/assets/shaders/generate-specular-map.comp -o %{cfg.buildtarget.directory}/assets/shaders/generate-specular-map.comp.spv",
        "{MKDIR} %{cfg.buildtarget.directory}/assets/icons",
        "{MKDIR} %{cfg.buildtarget.directory}/assets/fonts",
        "{COPYDIR} ../../editor/assets/icons %{cfg.buildtarget.directory}/assets/icons",
        "{COPYDIR} ../../editor/assets/fonts %{cfg.buildtarget.directory}/assets/fonts"
    }

    copyEngineAssets()

project "LiquidEditorTest"
    basedir "../workspace/editor-test"
    kind "ConsoleApp"

    configurations {
        "Debug", "Test"
    }

    includedirs {
        "../editor/tests",
        "../editor/src",
        "../engine/src",
        "../engine/rhi/mock/include"
    }

    files {
        "src/**.cpp",
        "tests/**.cpp",
        "tests/**.h"
    }

    removefiles {
        "src/main.cpp"
    }

    linkDependenciesWith{"LiquidEngine", "LiquidRHIMock", "LiquidRHICore", "vendor-libimguizmo", "vendor-libmikktspace", "meshoptimizer"}
    linkGoogleTest{}
    linkPlatform()

    copyEngineAssets()

    postbuildcommands {
        "{COPYDIR} ../../editor/tests/fixtures %{cfg.buildtarget.directory}/fixtures"
    }

