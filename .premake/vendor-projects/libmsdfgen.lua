project "vendor-libmsdfgen"
    basedir "../../vendor/projects/msdfgen"
    kind "StaticLib"

    files {
        "../../vendor/projects/msdfgen/*.cpp",
    }

    removefiles {
        "../../vendor/projects/msdfgen/import-svg.cpp",
        "../../vendor/projects/msdfgen/save-png.cpp"
    }

    vendorIncludes {
        "msdfgen/core",
        "msdfgen/ext"
    }

    filter { "toolset:msc-*" }
        removeflags { "FatalCompileWarnings" }
