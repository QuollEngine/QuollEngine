project "LiquidRHIMock"
    basedir "../../../workspace/rhi-mock"
    kind "StaticLib"

    includedirs {
        "include",
        "include/liquid/rhi-mock",
        "../base/include/liquid/rhi"
    }

    files {
        "src/*.cpp", 
        "include/**.h"
    }
