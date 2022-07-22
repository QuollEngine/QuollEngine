project "vendor-libmsdf-atlas-gen"
    basedir "../../vendor/projects/lua"
    kind "StaticLib"

    files {
        "../../vendor/projects/msdf-atlas-gen/*.cpp"
    }

    includedirs {
        "../../vendor/Debug/include/msdf-atlas-gen",
        "../../vendor/Debug/include/msdfgen"
    }

    filter { "toolset:msc-*" }
        removeflags { "FatalCompileWarnings" }
