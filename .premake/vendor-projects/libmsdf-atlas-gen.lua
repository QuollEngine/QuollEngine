project "vendor-libmsdf-atlas-gen"
    basedir "../../vendor/projects/msdf-atlas-gen"
    kind "StaticLib"

    files {
        "../../vendor/projects/msdf-atlas-gen/*.cpp"
    }

    vendorIncludes {
        "msdf-atlas-gen"
    }

    filter { "toolset:msc-*" }
        removeflags { "FatalCompileWarnings" }
