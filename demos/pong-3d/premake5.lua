project "DemoPong"
    basedir "../../workspace/demos/pong-3d"
    kind "ConsoleApp"
    configurations { "Debug" }
    files {
        "src/**.h",
        "src/**.cpp"
    }

    linkDependenciesWith{"LiquidEngine"}

    postbuildcommands {
        "glslc ../../../demos/pong-3d/assets/basic-shader.vert -o %{cfg.buildtarget.directory}/basic-shader.vert.spv",
        "glslc ../../../demos/pong-3d/assets/basic-shader.frag -o %{cfg.buildtarget.directory}/basic-shader.frag.spv"
    }
