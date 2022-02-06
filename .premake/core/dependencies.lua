-- Link profiler dependencies
function linkProfilerDependencies()
    filter { "configurations:Profile-Debug or configurations:Profile-Release" }

        filter { "system:windows" }
            links { "OptickCored" }
            postbuildcommands {
                '{COPY} %{wks.location}/../vendor/bin/OptickCored.dll %{cfg.buildtarget.directory}'
            }
            
        filter {"system:linux or system:macosx"}
            links { "OptickCore" }

    filter{}
end

-- Link dependencies without Vulkan
function linkDependenciesWithoutVulkan()
    links { "vendor-libimgui", "vendor-libspirv-reflect", "glfw3", "ktx" }

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
    filter { "system:windows" }
        links { "gtestd", "gtest_maind", "gmockd" }

    filter { "system:macosx or system:linux" }
        links { "gtest_main", "gtest", "gmock" }

    filter{}
end
