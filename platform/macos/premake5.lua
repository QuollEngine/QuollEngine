function linkPlatform()
    links { "QuollPlatformMacOS" }
end

project "QuollPlatformMacOS"
    basedir "../../workspace/platform-macos"
    kind "StaticLib"

    includedirs {
        "include/"
    }

    files {
        "src/**.mm"
    }
