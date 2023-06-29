function linkPlatform()
    links { 'LiquidPlatformWin32' }
end

project 'LiquidPlatformWin32'
    basedir "../../workspace/platform-win32"
    kind "StaticLib"

    includedirs {
        "include/"
    }

    files {
        "src/**.cpp"
    }
