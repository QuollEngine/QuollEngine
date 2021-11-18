-- Setup platform specific defines
function setupPlatformDefines()
    filter { "system:windows" }
        defines {
            "LIQUID_PLATFORM_WINDOWS"
        }

    filter { "system:macosx" }
        defines {
            "LIQUID_PLATFORM_MACOS"
        }

    filter { "system:linux" }
        defines {
            "LIQUID_PLATFORM_LINUX"
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
