project "vendor-libmsdfgen"
    basedir "../../vendor/projects/lua"
    kind "StaticLib"

    files {
        "../../vendor/projects/msdfgen/*.cpp",
    }

    removefiles {
        "../../vendor/projects/msdfgen/import-svg.cpp",
        "../../vendor/projects/msdfgen/save-png.cpp"
    }

    includedirs {
        "../../vendor/Debug/include/msdfgen/core",
        "../../vendor/Debug/include/msdfgen/ext"
    }

    filter { "toolset:msc-*" }
        removeflags { "FatalCompileWarnings" }
