function linkPlatform()
    links { "LiquidPlatformLinux" }
end

project "LiquidPlatformLinux"
    basedir "../../workspace/platform-linux"
    kind "StaticLib"

    includedirs {
        "include/"
    }

    files {
        "src/**.cpp"
    }
