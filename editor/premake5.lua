project "Liquidator"
    basedir "../workspace/editor"
    kind "ConsoleApp"

    includedirs {
        "./src"
    }

    loadSourceFiles{}
    linkDependenciesWith{"LiquidEngine", "LiquidEngineRHIVulkan", "LiquidEngineRHICore", "LiquidEnginePlatformTools", "vendor-libimguizmo"}
    
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
        "{COPYFILE} ../../editor/assets/icons/texture.png %{cfg.buildtarget.directory}/assets/icons/texture.png",
        "{COPYFILE} ../../editor/assets/icons/font.png %{cfg.buildtarget.directory}/assets/icons/font.png",
        "{COPYFILE} ../../editor/assets/icons/material.png %{cfg.buildtarget.directory}/assets/icons/material.png",
        "{COPYFILE} ../../editor/assets/icons/mesh.png %{cfg.buildtarget.directory}/assets/icons/mesh.png",
        "{COPYFILE} ../../editor/assets/icons/skeleton.png %{cfg.buildtarget.directory}/assets/icons/skeleton.png",
        "{COPYFILE} ../../editor/assets/icons/animation.png %{cfg.buildtarget.directory}/assets/icons/animation.png",
        "{COPYFILE} ../../editor/assets/icons/audio.png %{cfg.buildtarget.directory}/assets/icons/audio.png",
        "{COPYFILE} ../../editor/assets/icons/prefab.png %{cfg.buildtarget.directory}/assets/icons/prefab.png",
        "{COPYFILE} ../../editor/assets/icons/directory.png %{cfg.buildtarget.directory}/assets/icons/directory.png",
        "{COPYFILE} ../../editor/assets/icons/create-directory.png %{cfg.buildtarget.directory}/assets/icons/create-directory.png",
        "{COPYFILE} ../../editor/assets/icons/unknown.png %{cfg.buildtarget.directory}/assets/icons/unknown.png",
        "{COPYFILE} ../../editor/assets/icons/script.png %{cfg.buildtarget.directory}/assets/icons/script.png",
        "{COPYFILE} ../../editor/assets/icons/sun.png %{cfg.buildtarget.directory}/assets/icons/sun.png",
        "{COPYFILE} ../../editor/assets/icons/direction.png %{cfg.buildtarget.directory}/assets/icons/direction.png",
        "{COPYFILE} ../../editor/assets/icons/camera.png %{cfg.buildtarget.directory}/assets/icons/camera.png",
        "{COPYFILE} ../../editor/assets/icons/play.png %{cfg.buildtarget.directory}/assets/icons/play.png",
        "{COPYFILE} ../../editor/assets/icons/stop.png %{cfg.buildtarget.directory}/assets/icons/stop.png",
        "{COPYFILE} ../../editor/assets/icons/move.png %{cfg.buildtarget.directory}/assets/icons/move.png",
        "{COPYFILE} ../../editor/assets/icons/rotate.png %{cfg.buildtarget.directory}/assets/icons/rotate.png",
        "{COPYFILE} ../../editor/assets/icons/scale.png %{cfg.buildtarget.directory}/assets/icons/scale.png",
    }

    copyEngineAssets()
