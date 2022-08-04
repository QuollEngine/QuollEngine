-- Setup all library directories
function setupLibraryDirectories()
    -- This define is needed for libktx to reference
    -- static library functions instead of DLL
    defines {
        "KHRONOS_STATIC"
    }

    filter { "configurations:Debug" }
        sysincludedirs {
            "../vendor/Debug/include",
            "../vendor/Debug/include/freetype2",
            "../vendor/Debug/include/msdfgen"
        }
        libdirs { "../vendor/Debug/lib", "../vendor/Debug/lib/debug" }
    
    filter { "configurations:Release or configurations:Profile" }
        sysincludedirs {
            "../vendor/Release/include",
            "../vendor/Release/include/freetype2",
            "../vendor/Release/include/msdfgen"
        }
        libdirs { "../vendor/Release/lib", "../vendor/Release/lib/release" }

    filter { "system:windows" }
        -- Vulkan SDK for Windows adds environment
        -- variable that points to Vulkan SDK
        -- installation
        sysincludedirs {
            "$(VULKAN_SDK)/Include"
        }

        libdirs {
            "$(VULKAN_SDK)/Lib"
        }

    filter { "system:macosx" }
        -- Vulkan SDK for macOS is installed to
        -- global include, library, and bin dirs
        sysincludedirs {
            "/usr/local/include"
        }

        libdirs {
            "/usr/local/lib"
        }

    filter{}
end
