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
        "glslc "..assetsPath.."/shaders/sprite.vert -o"..outputPath.."/shaders/sprite.vert.spv",
        "glslc "..assetsPath.."/shaders/sprite.frag -o"..outputPath.."/shaders/sprite.frag.spv",
        "glslc "..assetsPath.."/shaders/fullscreen-quad.frag -o"..outputPath.."/shaders/fullscreen-quad.frag.spv",
        "glslc "..assetsPath.."/shaders/fullscreen-quad.vert -o"..outputPath.."/shaders/fullscreen-quad.vert.spv",
        "glslc "..assetsPath.."/shaders/generate-brdf-lut.comp -o "..outputPath.."/shaders/generate-brdf-lut.comp.spv",
        "glslc "..assetsPath.."/shaders/extract-bright-colors.comp -o "..outputPath.."/shaders/extract-bright-colors.comp.spv",
        "glslc "..assetsPath.."/shaders/bloom-downsample.comp -o "..outputPath.."/shaders/bloom-downsample.comp.spv",
        "glslc "..assetsPath.."/shaders/bloom-upsample.comp -o "..outputPath.."/shaders/bloom-upsample.comp.spv",
        "glslc "..assetsPath.."/shaders/hdr.frag -o "..outputPath.."/shaders/hdr.frag.spv",

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
        "../engine/src",
        "../engine/rhi/mock/include"
    }

    files {
        "tests/**.cpp",
        "tests/**.h"
    }

    linkDependenciesWith{"LiquidEngine", "LiquidRHIMock", "LiquidRHICore"}
    linkGoogleTest{}

    postbuildcommands {
        "{COPYDIR} ../../engine/tests/fixtures %{cfg.buildtarget.directory}/fixtures"
    }

