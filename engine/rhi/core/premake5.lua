project "LiquidRHICore"
    basedir "../../../workspace/rhi-core"
    kind "StaticLib"

    includedirs {
        "include",
        "include/liquid/rhi"
    }

    files {
        "src/*.cpp", 
        "include/**.h"
    }
