project "Liquidator"
    basedir "../workspace/editor"
    kind "ConsoleApp"
    configurations { "Debug" }

    loadSourceFiles{}
    linkDependenciesWith{"LiquidEngine", "LiquidEnginePlatformTools"}

    postbuildcommands {
        "{MKDIR} %{cfg.buildtarget.directory}/assets/shaders/",
        "glslc ../../editor/assets/shaders/editor-grid.vert -o %{cfg.buildtarget.directory}/assets/shaders/editor-grid.vert.spv",
        "glslc ../../editor/assets/shaders/editor-grid.frag -o %{cfg.buildtarget.directory}/assets/shaders/editor-grid.frag.spv"
    }
