project "LiquidEngine"
    basedir "../workspace/engine"
    kind "StaticLib"

    pchheader "../../engine/src/liquid/core/Base.h"

    filter { "toolset:msc-*" }
        pchheader "liquid/core/Base.h"
        pchsource "src/liquid/core/Base.cpp"

    filter{}

    loadSourceFiles{}
    linkDependenciesWith{}

    postbuildcommands {
        "{MKDIR} %{cfg.buildtarget.directory}/assets/shaders/",
        "glslc ../../engine/assets/shaders/geometry.vert -o %{cfg.buildtarget.directory}/assets/shaders/geometry.vert.spv",
        "glslc ../../engine/assets/shaders/skinnedGeometry.vert -o %{cfg.buildtarget.directory}/assets/shaders/skinnedGeometry.vert.spv",
        "glslc ../../engine/assets/shaders/pbr.frag -o %{cfg.buildtarget.directory}/assets/shaders/pbr.frag.spv",
        "glslc ../../engine/assets/shaders/skybox.frag -o %{cfg.buildtarget.directory}/assets/shaders/skybox.frag.spv",
        "glslc ../../engine/assets/shaders/skybox.vert -o %{cfg.buildtarget.directory}/assets/shaders/skybox.vert.spv",
        "glslc ../../engine/assets/shaders/shadowmap.frag -o %{cfg.buildtarget.directory}/assets/shaders/shadowmap.frag.spv",
        "glslc ../../engine/assets/shaders/shadowmap.vert -o %{cfg.buildtarget.directory}/assets/shaders/shadowmap.vert.spv", 
        "glslc ../../engine/assets/shaders/skinnedShadowmap.vert -o %{cfg.buildtarget.directory}/assets/shaders/skinnedShadowmap.vert.spv",
        "glslc ../../engine/assets/shaders/imgui.frag -o %{cfg.buildtarget.directory}/assets/shaders/imgui.frag.spv",
        "glslc ../../engine/assets/shaders/imgui.vert -o %{cfg.buildtarget.directory}/assets/shaders/imgui.vert.spv",
        "glslc ../../engine/assets/shaders/fullscreenQuad.frag -o %{cfg.buildtarget.directory}/assets/shaders/fullscreenQuad.frag.spv",
        "glslc ../../engine/assets/shaders/fullscreenQuad.vert -o %{cfg.buildtarget.directory}/assets/shaders/fullscreenQuad.vert.spv"
    }

project "LiquidEngineTest"
    basedir "../workspace/engine-test"
    kind "ConsoleApp"

    -- Disable profiler in tests
    undefines { "LIQUID_PROFILER" }

    pchheader "../../engine/src/liquid/core/Base.h"

    filter { "toolset:msc-*" }
        pchheader "liquid/core/Base.h"
        pchsource "src/liquid/core/Base.cpp"

    filter{}

    files {
        "src/**.cpp",
        "src/**.h",
        "tests/**.cpp",
        "tests/**.h"
    }

    removefiles {
        "src/liquid/renderer/vulkan/VmaImpl.cpp",
    }

    setupTestingOptions{}
    linkGoogleTest{}
    linkDependenciesWithoutVulkan{}

    postbuildcommands {
        "{COPYFILE} ../../engine/tests/fixtures/white-image-100x100.png %{cfg.buildtarget.directory}/white-image-100x100.png",
        "{COPYFILE} ../../engine/tests/fixtures/1x1-cubemap.ktx %{cfg.buildtarget.directory}/1x1-cubemap.ktx",
        "{COPYFILE} ../../engine/tests/fixtures/1x1-2d.ktx %{cfg.buildtarget.directory}/1x1-2d.ktx",
        "{COPYFILE} ../../engine/tests/fixtures/1x1-1d.ktx %{cfg.buildtarget.directory}/1x1-1d.ktx"
    }
