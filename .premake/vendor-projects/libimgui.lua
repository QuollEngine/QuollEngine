project "vendor-libimgui"
    basedir "../../vendor/projects/imgui"
    kind "StaticLib"

    files {
        "../../vendor/projects/imgui/*.cpp",
    }
