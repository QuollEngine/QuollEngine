project "LiquidRHICore"
    basedir "../../../workspace/rhi-core"
    kind "StaticLib"

    includedirs {
        "include",
        "include/quoll/rhi"
    }

    files {
        "src/*.cpp", 
        "include/**.h"
    }
