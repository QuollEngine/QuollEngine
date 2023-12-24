project "QuollRHIMock"
    basedir "../../../workspace/rhi-mock"
    kind "StaticLib"

    includedirs {
        "include",
        "include/quoll/rhi-mock",
        "../base/include/quoll/rhi"
    }

    files {
        "src/*.cpp", 
        "include/**.h"
    }
