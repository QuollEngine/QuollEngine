project "Liquidator"
    basedir "../workspace/editor"
    kind "ConsoleApp"
    configurations { "Debug" }

    files {
        "src/**.h",
        "src/**.cpp"
    }

    linkDependenciesWith{"LiquidEngine", "LiquidEnginePlatformTools"}
