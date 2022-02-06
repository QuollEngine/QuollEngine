project "DemoBasicTriangle"
    basedir "../../workspace/demos/basic-triangle"
    kind "ConsoleApp"

    loadSourceFiles{}
    linkDependenciesWith{"LiquidEngine"}

    postbuildcommands {
        "glslc ../../../demos/basic-triangle/assets/basic-shader.vert -o %{cfg.buildtarget.directory}/basic-shader.vert.spv",
        "glslc ../../../demos/basic-triangle/assets/basic-shader.frag -o %{cfg.buildtarget.directory}/basic-shader.frag.spv",
        "glslc ../../../demos/basic-triangle/assets/red-shader.frag -o %{cfg.buildtarget.directory}/red-shader.frag.spv",
        "glslc ../../../demos/basic-triangle/assets/texture-shader.frag -o %{cfg.buildtarget.directory}/texture-shader.frag.spv",
        "glslc ../../../demos/basic-triangle/assets/texture-shader.vert -o %{cfg.buildtarget.directory}/texture-shader.vert.spv",
        "{COPYFILE} ../../../demos/basic-triangle/assets/textures/brick.png %{cfg.buildtarget.directory}/brick.png"
    }
