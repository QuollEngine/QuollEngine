-- Setup all library directories
function setupLibraryDirectories()
    -- This define is needed for libktx to reference
    -- static library functions instead of DLL
    defines {
        "KHRONOS_STATIC"
    }

    sysincludedirs {
        "../vendor/include"
    }

    libdirs {
        "../vendor/lib",
        "../vendor/lib/debug"
    }

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
