project "DemoSceneViewer"
    basedir "../../workspace/demos/scene-viewer"
    kind "ConsoleApp"

    loadSourceFiles{}
    linkDependenciesWith{"LiquidEngine", "LiquidEnginePlatformTools"}

    copyEngineAssets("../../../engine/assets", "%{cfg.buildtarget.directory}/engine/assets")
