-- Setup platform specific defines
function setupPlatformDefines()
    filter { "system:windows" }
        defines {
            "QUOLL_PLATFORM_WINDOWS"
        }

    filter { "system:macosx" }
        defines {
            "QUOLL_PLATFORM_MACOS"
        }

    filter { "system:linux" }
        defines {
            "QUOLL_PLATFORM_LINUX"
        }

    filter{}
 end

-- Link platform libraries
function linkPlatformLibraries()
    filter { "system:macosx" }
        -- GLFW is linked statically; so, we need to 
        -- link the necessary frameworks
        links {
            "Cocoa.framework",
            "CoreFoundation.framework",
            "IOKit.framework",
            "CoreVideo.framework",
            "AppKit.framework"
        }

    filter { "system:linux" }
        linkoptions { "-pthread" }

    filter{}
end

-- Get target extension based
-- on platform
function getTargetExtension(targetName)
    return targetName .. "%{iif(cfg.system == 'windows', '.exe', '')}"
end
