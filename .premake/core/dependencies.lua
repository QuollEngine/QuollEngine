-- Appends debug suffix to
-- linked libraries automatically
function linkSuffixed(...)
    function appendDebug(arr)
        local t = {}
        for k, v in pairs(arr) do
            t[k] = v .. 'd'
        end
    
        return t
    end

    filter { "configurations:Debug or configurations:Test" }
        links(appendDebug(...))

    filter { "configurations:Release or configurations:Profile" }
        links { ... }

    filter{}
end

-- Link dependencies without Vulkan
function linkDependenciesWith(...)
    links { ... }

    links {
        "spirv-reflect",
        "lua",
        "vendor-libmsdf-atlas-gen",
        "vendor-libmsdfgen",
        "glfw3",
        "cryptopp",
        "ktx",
        "zstd",
        "PhysX_static_64",
        "PhysXPvdSDK_static_64",
        "PhysXExtensions_static_64",
        "PhysXCommon_static_64",
        "PhysXFoundation_static_64"
    }

    linkSuffixed {
        "yaml-cpp", "imgui", "freetype"
    }
    
    -- These libs must be linked after
    -- all libraries are linked
    filter { "system:linux" }
        links { "Xrandr", "Xi", "X11", "dl" }

    filter { "system:windows" }
        links { "dwmapi" }

    filter{}
end

-- Link Google Test
function linkGoogleTest()
    links { "gtest", "gmock" }
end

-- Link profiler dependencies
function linkOptick()
    filter { "configurations:Profile" }
        links { "OptickCore" }

    filter { "configurations:Profile", "system:windows" }
        postbuildcommands {
            '{COPY} %{wks.location}/../vendor/Release/bin/OptickCore.dll %{cfg.buildtarget.directory}'
        }

    filter{}
end
