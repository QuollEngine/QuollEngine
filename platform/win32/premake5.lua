function linkPlatform()
    links { 'QuollPlatformWin32' }
end

project 'QuollPlatformWin32'
    basedir "../../workspace/platform-win32"
    kind "StaticLib"

    includedirs {
        "include/"
    }

    files {
        "src/**.cpp"
    }
