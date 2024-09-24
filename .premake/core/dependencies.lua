function appendDebug(arr)
    local t = {}
    for k, v in pairs(arr) do
        t[k] = v .. 'd'
    end

    return t
end

-- Appends debug suffix to
-- linked libraries automatically
function linkSuffixed(...)
    filter { "configurations:Debug or configurations:Test" }
        links(appendDebug(...))

    filter { "configurations:Release or configurations:Profile" }
        links { ... }

    filter{}
end

-- Appends debug suffix to
-- linked libraries automatically
function linkSuffixedSystem(system, ...)
    filter { "system:" .. system, "configurations:Debug or configurations:Test" }
        links(appendDebug(...))

    filter { "system:" .. system, "configurations:Release or configurations:Profile" }
        links { ... }

    filter{}
end

-- Link dependencies without Vulkan
function linkDependenciesWith(...)
    links { ... }

    links {
        "msdf-atlas-gen",
        "msdfgen-core",
        "msdfgen-ext",
    }

    linkSuffixed {
        "yaml-cpp", "imgui", "implot", "freetype", "bz2"
    }

    links {
        "spirv-reflect-static",
        "lua",
        "glfw3",
        "cryptopp",
        "ktx",
        "zstd",
        "yogacore",
        "flecs_static",
        "brotlidec",
        "brotlicommon",
        "PhysX_static_64",
        "PhysXPvdSDK_static_64",
        "PhysXExtensions_static_64",
        "PhysXCommon_static_64",
        "PhysXFoundation_static_64"
    }

    linkSuffixedSystem("linux", {"png16"})
    linkSuffixedSystem("windows", {"zlib", "libpng16"})
    
    -- These libs must be linked after
    -- all libraries are linked
    filter { "system:linux" }
        links { "z" }
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
function linkTracy()
    filter { "configurations:Profile" }
        links { "TracyClient" }

    filter{}
end
