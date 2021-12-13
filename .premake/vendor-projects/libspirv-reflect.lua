project "vendor-libspirv-reflect"
    basedir "../../vendor/projects/spirv-reflect"
    kind "StaticLib"

    files {
        "../../vendor/projects/spirv-reflect/*.c",
    }
