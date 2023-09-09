project "vendor-libmsdf-atlas-gen"
    basedir "../../vendor/projects/lua"
    kind "StaticLib"

    files {
        "../../vendor/projects/msdf-atlas-gen/*.cpp"
    }

    vendorIncludes {
        "msdf-atlas-gen"
    }

    filter { "toolset:msc-*" }
        removeflags { "FatalCompileWarnings" }
