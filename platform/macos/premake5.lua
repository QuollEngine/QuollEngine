function linkPlatform()
    links { "LiquidPlatformMacOS" }
end

project "LiquidPlatformMacOS"
    basedir "../../workspace/platform-macos"
    kind "StaticLib"

    includedirs {
        "include/"
    }

    files {
        "src/**.mm"
    }
