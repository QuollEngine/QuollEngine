function linkPlatform()
    links { "QuollPlatformLinux" }
end

project "QuollPlatformLinux"
    basedir "../../workspace/platform-linux"
    kind "StaticLib"

    includedirs {
        "include/"
    }

    files {
        "src/**.cpp"
    }
