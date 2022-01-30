project "DemoSceneViewer"
    basedir "../../workspace/demos/scene-viewer"
    kind "ConsoleApp"
    configurations { "Debug" }

    loadSourceFiles{}
    linkDependenciesWith{"LiquidEngine", "LiquidEnginePlatformTools"}
