project "DemoSceneViewer"
    basedir "../../workspace/demos/scene-viewer"
    kind "ConsoleApp"
    configurations { "Debug" }
    files {
        "src/**.h",
        "src/**.cpp"
    }

    linkDependenciesWith{"LiquidEngine", "LiquidEnginePlatformTools"}
