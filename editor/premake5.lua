project "Liquidator"
    basedir "../workspace/editor"
    kind "ConsoleApp"

    loadSourceFiles{}
    linkDependenciesWith{"LiquidEngine", "LiquidEnginePlatformTools"}

    postbuildcommands {
        "{MKDIR} %{cfg.buildtarget.directory}/assets/shaders/",
        "glslc ../../editor/assets/shaders/editor-grid.vert -o %{cfg.buildtarget.directory}/assets/shaders/editor-grid.vert.spv",
        "glslc ../../editor/assets/shaders/editor-grid.frag -o %{cfg.buildtarget.directory}/assets/shaders/editor-grid.frag.spv",
        "glslc ../../editor/assets/shaders/skeleton-lines.vert -o %{cfg.buildtarget.directory}/assets/shaders/skeleton-lines.vert.spv",
        "glslc ../../editor/assets/shaders/skeleton-lines.frag -o %{cfg.buildtarget.directory}/assets/shaders/skeleton-lines.frag.spv",
        "glslc ../../editor/assets/shaders/object-icons.vert -o %{cfg.buildtarget.directory}/assets/shaders/object-icons.vert.spv",
        "glslc ../../editor/assets/shaders/object-icons.frag -o %{cfg.buildtarget.directory}/assets/shaders/object-icons.frag.spv",
        "{MKDIR} %{cfg.buildtarget.directory}/assets/icons",
        "{COPYFILE} ../../editor/assets/icons/texture.png %{cfg.buildtarget.directory}/assets/icons/texture.png",
        "{COPYFILE} ../../editor/assets/icons/material.png %{cfg.buildtarget.directory}/assets/icons/material.png",
        "{COPYFILE} ../../editor/assets/icons/mesh.png %{cfg.buildtarget.directory}/assets/icons/mesh.png",
        "{COPYFILE} ../../editor/assets/icons/skeleton.png %{cfg.buildtarget.directory}/assets/icons/skeleton.png",
        "{COPYFILE} ../../editor/assets/icons/animation.png %{cfg.buildtarget.directory}/assets/icons/animation.png",
        "{COPYFILE} ../../editor/assets/icons/prefab.png %{cfg.buildtarget.directory}/assets/icons/prefab.png",
        "{COPYFILE} ../../editor/assets/icons/directory.png %{cfg.buildtarget.directory}/assets/icons/directory.png",
        "{COPYFILE} ../../editor/assets/icons/unknown.png %{cfg.buildtarget.directory}/assets/icons/unknown.png",
        "{COPYFILE} ../../editor/assets/icons/sun.png %{cfg.buildtarget.directory}/assets/icons/sun.png",
        "{COPYFILE} ../../editor/assets/icons/direction.png %{cfg.buildtarget.directory}/assets/icons/direction.png"
    }

    copyEngineAssets("../../engine/assets", "%{cfg.buildtarget.directory}/engine/assets")
