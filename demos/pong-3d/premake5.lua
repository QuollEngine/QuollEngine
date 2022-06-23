project "DemoPong"
    basedir "../../workspace/demos/pong-3d"
    kind "ConsoleApp"

    loadSourceFiles{}
    linkDependenciesWith{"LiquidEngine", "LiquidEngineRHIVulkan", "LiquidEngineRHICore"}

    postbuildcommands {
        "glslc ../../../demos/pong-3d/assets/basic-shader.vert -o %{cfg.buildtarget.directory}/basic-shader.vert.spv",
        "glslc ../../../demos/pong-3d/assets/basic-shader.frag -o %{cfg.buildtarget.directory}/basic-shader.frag.spv",
        "{COPYFILE} ../../../demos/pong-3d/assets/meshes/cube.lqmesh %{cfg.buildtarget.directory}/cube.lqmesh",
        "{COPYFILE} ../../../demos/pong-3d/assets/meshes/sphere.lqmesh %{cfg.buildtarget.directory}/sphere.lqmesh",
        "{COPYFILE} ../../../demos/pong-3d/assets/scripts/player.lua %{cfg.buildtarget.directory}/player.lua"
    }

    copyEngineAssets("../../../engine/assets", "%{cfg.buildtarget.directory}/engine/assets")

