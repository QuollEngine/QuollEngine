project "vendor-liblua"
    basedir "../../vendor/projects/lua"
    kind "StaticLib"

    files {
        "../../vendor/projects/lua/*.c",
    }

    removefiles {
        "../../vendor/projects/lua/lua.c",
        "../../vendor/projects/lua/onelua.c"
    }
