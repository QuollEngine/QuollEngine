-- Setup all library directories
function setupLibraryDirectories()
    -- This define is needed for libktx to reference
    -- static library functions instead of DLL
    defines {
        "KHRONOS_STATIC"
    }

    filter { "configurations:Debug or configurations:Test" }
        externalincludedirs {
            "../vendor/Debug/include",
            "../vendor/Debug/include/freetype2",
            "../vendor/Debug/include/msdfgen"
        }
        libdirs { "../vendor/Debug/lib", "../vendor/Debug/lib/debug" }
    
    filter { "configurations:Release or configurations:Profile" }
        externalincludedirs {
            "../vendor/Release/include",
            "../vendor/Release/include/freetype2",
            "../vendor/Release/include/msdfgen"
        }
        libdirs { "../vendor/Release/lib", "../vendor/Release/lib/release" }

    filter { "system:windows" }
        -- Vulkan SDK for Windows adds environment
        -- variable that points to Vulkan SDK
        -- installation
        externalincludedirs {
            "$(VULKAN_SDK)/Include"
        }

    filter { "system:macosx" }
        -- Vulkan SDK for macOS is installed to
        -- global include, library, and bin dirs
        externalincludedirs {
            "/usr/local/include"
        }

    filter{}
end
