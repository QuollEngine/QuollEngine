-- Link profiler dependencies
function linkProfilerDependencies()
    filter { "configurations:Profile" }
        links { "OptickCore" }

    filter { "configurations:Profile", "system:windows" }
        postbuildcommands {
            '{COPY} %{wks.location}/../vendor/Release/bin/OptickCore.dll %{cfg.buildtarget.directory}'
        }

    filter{}
end

-- Link dependencies without Vulkan
function linkDependenciesWithoutVulkan()
    links {
        "vendor-libimgui",
        "vendor-libspirv-reflect",
        "vendor-liblua",
        "vendor-libmsdf-atlas-gen",
        "vendor-libmsdfgen",
        "glfw3",
        "ktx",
        "PhysX_static",
        "PhysXPvdSDK_static",
        "PhysXExtensions_static",
        "PhysXCommon_static",
        "PhysXFoundation_static"
    }

    filter { "system:windows", "configurations:Debug" }
        links { "yaml-cppd", "freetyped" }

    filter { "system:windows", "configurations:Release or configurations:Profile" }
        links { "yaml-cpp", "freetype" }

    filter {"system:linux or system:macosx"}
        links { "yaml-cpp", "freetype" }

    -- These libs must be linked after
    -- all libraries are linked
    filter { "system:linux" }
        links { "Xrandr", "Xi", "X11", "dl" }

    filter{}
end

-- Link dependencies shared by all projects
function linkDependenciesWith(...)
    links { ... }

    filter { "system:windows" }
        links { "vulkan-1" }

    filter { "system:linux or system:macosx" }
        links { "vulkan" }
    
    filter{}

    linkDependenciesWithoutVulkan{}
    linkProfilerDependencies{}

    filter{}
end

-- Link Google Test
function linkGoogleTest()
    filter { "system:windows", "configurations:Debug" }
        links { "gtestd", "gtest_maind", "gmockd" }

    filter {"system:windows", "configurations:Release"}
        links { "gtest", "gtest_main", "gmock" }

    filter { "system:macosx or system:linux" }
        links { "gtest_main", "gtest", "gmock" }

    filter{}
end
