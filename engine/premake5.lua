function copyEngineAssets(assetsPath, outputPath)
    postbuildcommands {
        "{MKDIR} %{cfg.buildtarget.directory}/engine/assets/shaders/",
        "glslc "..assetsPath.."/shaders/geometry.vert -o "..outputPath.."/shaders/geometry.vert.spv",
        "glslc "..assetsPath.."/shaders/skinnedGeometry.vert -o"..outputPath.."/shaders/skinnedGeometry.vert.spv",
        "glslc "..assetsPath.."/shaders/pbr.frag -o"..outputPath.."/shaders/pbr.frag.spv",
        "glslc "..assetsPath.."/shaders/skybox.frag -o"..outputPath.."/shaders/skybox.frag.spv",
        "glslc "..assetsPath.."/shaders/skybox.vert -o"..outputPath.."/shaders/skybox.vert.spv",
        "glslc "..assetsPath.."/shaders/shadowmap.frag -o"..outputPath.."/shaders/shadowmap.frag.spv",
        "glslc "..assetsPath.."/shaders/shadowmap.vert -o"..outputPath.."/shaders/shadowmap.vert.spv", 
        "glslc "..assetsPath.."/shaders/skinnedShadowmap.vert -o"..outputPath.."/shaders/skinnedShadowmap.vert.spv",
        "glslc "..assetsPath.."/shaders/imgui.frag -o"..outputPath.."/shaders/imgui.frag.spv",
        "glslc "..assetsPath.."/shaders/imgui.vert -o"..outputPath.."/shaders/imgui.vert.spv",
        "glslc "..assetsPath.."/shaders/fullscreenQuad.frag -o"..outputPath.."/shaders/fullscreenQuad.frag.spv",
        "glslc "..assetsPath.."/shaders/fullscreenQuad.vert -o"..outputPath.."/shaders/fullscreenQuad.vert.spv"
    }
end

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

project "LiquidEngineTest"
    basedir "../workspace/engine-test"
    kind "ConsoleApp"

    configmap {
        ["Release"] = "Debug",
        ["Profile-Release"] = "Debug",
        ["Profile-Debug"] = "Debug"
    }

    pchheader "../../engine/src/liquid/core/Base.h"

    includedirs {
        "../engine/tests",
    }

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
        "src/liquid/rhi/vulkan/VmaImpl.cpp",
    }

    setupTestingOptions{}
    linkGoogleTest{}
    linkDependenciesWithoutVulkan{}

    postbuildcommands {
        "{COPYFILE} ../../engine/tests/fixtures/white-image-100x100.png %{cfg.buildtarget.directory}/white-image-100x100.png",
        "{COPYFILE} ../../engine/tests/fixtures/1x1-cubemap.ktx %{cfg.buildtarget.directory}/1x1-cubemap.ktx",
        "{COPYFILE} ../../engine/tests/fixtures/1x1-2d.ktx %{cfg.buildtarget.directory}/1x1-2d.ktx",
        "{COPYFILE} ../../engine/tests/fixtures/1x1-1d.ktx %{cfg.buildtarget.directory}/1x1-1d.ktx",
        "{COPYFILE} ../../engine/tests/fixtures/component-script.lua %{cfg.buildtarget.directory}/component-script.lua",
        "{COPYFILE} ../../engine/tests/fixtures/component-script-2.lua %{cfg.buildtarget.directory}/component-script-2.lua",
        "{COPYFILE} ../../engine/tests/fixtures/scripting-system-tester.lua %{cfg.buildtarget.directory}/scripting-system-tester.lua",
        "{COPYFILE} ../../engine/tests/fixtures/scripting-system-component-tester.lua %{cfg.buildtarget.directory}/scripting-system-component-tester.lua"
    }
