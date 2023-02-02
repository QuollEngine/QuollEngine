function copyEngineAssets()
    assetsPath = "../../engine/assets"
    outputPath = "%{cfg.buildtarget.directory}/engine/assets"

    postbuildcommands {
        "{MKDIR} "..outputPath.."/shaders/",
        -- Shaders
        "glslc "..assetsPath.."/shaders/geometry.vert -o "..outputPath.."/shaders/geometry.vert.spv",
        "glslc "..assetsPath.."/shaders/geometry-skinned.vert -o"..outputPath.."/shaders/geometry-skinned.vert.spv",
        "glslc "..assetsPath.."/shaders/pbr.frag -o"..outputPath.."/shaders/pbr.frag.spv",
        "glslc "..assetsPath.."/shaders/skybox.frag -o"..outputPath.."/shaders/skybox.frag.spv",
        "glslc "..assetsPath.."/shaders/skybox.vert -o"..outputPath.."/shaders/skybox.vert.spv",
        "glslc "..assetsPath.."/shaders/shadowmap.frag -o"..outputPath.."/shaders/shadowmap.frag.spv",
        "glslc "..assetsPath.."/shaders/shadowmap.vert -o"..outputPath.."/shaders/shadowmap.vert.spv", 
        "glslc "..assetsPath.."/shaders/shadowmap-skinned.vert -o"..outputPath.."/shaders/shadowmap-skinned.vert.spv",
        "glslc "..assetsPath.."/shaders/imgui.frag -o"..outputPath.."/shaders/imgui.frag.spv",
        "glslc "..assetsPath.."/shaders/imgui.vert -o"..outputPath.."/shaders/imgui.vert.spv",
        "glslc "..assetsPath.."/shaders/text.vert -o"..outputPath.."/shaders/text.vert.spv",
        "glslc "..assetsPath.."/shaders/text.frag -o"..outputPath.."/shaders/text.frag.spv",
        "glslc "..assetsPath.."/shaders/fullscreen-quad.frag -o"..outputPath.."/shaders/fullscreen-quad.frag.spv",
        "glslc "..assetsPath.."/shaders/fullscreen-quad.vert -o"..outputPath.."/shaders/fullscreen-quad.vert.spv",
        -- Fonts
        "{MKDIR} "..outputPath.."/fonts/",
        "{COPYFILE} "..assetsPath.."/fonts/Roboto-Regular.ttf "..outputPath.."/fonts"
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

    configurations {
        "Debug", "Test"
    }

    includedirs {
        "../engine/tests",
        "../engine/src"
    }

    files {
        "tests/**.cpp",
        "tests/**.h"
    }

    links { "LiquidEngine", "LiquidRHICore" }
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
        "{COPYFILE} ../../engine/tests/fixtures/scripting-system-component-tester.lua %{cfg.buildtarget.directory}/scripting-system-component-tester.lua",
        "{COPYFILE} ../../engine/tests/fixtures/scripting-system-logging-tester.lua %{cfg.buildtarget.directory}/scripting-system-logging-tester.lua",
        "{COPYFILE} ../../engine/tests/fixtures/scripting-system-no-start.lua %{cfg.buildtarget.directory}/scripting-system-no-start.lua",
        "{COPYFILE} ../../engine/tests/fixtures/scripting-system-no-update.lua %{cfg.buildtarget.directory}/scripting-system-no-update.lua",
        "{COPYFILE} ../../engine/tests/fixtures/scripting-system-invalid-syntax.lua %{cfg.buildtarget.directory}/scripting-system-invalid-syntax.lua",
        "{COPYFILE} ../../engine/tests/fixtures/valid-audio.wav %{cfg.buildtarget.directory}/valid-audio.wav",
        "{COPYFILE} ../../engine/tests/fixtures/valid-audio.mp3 %{cfg.buildtarget.directory}/valid-audio.mp3"
    }

