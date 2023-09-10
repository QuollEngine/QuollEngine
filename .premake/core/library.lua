function mapDirNames(arr, basePath)
    local t = {}
    for k, v in pairs(arr) do
        t[k] = basePath .. "/" .. v
    end

    return t
end

-- Set vendor includes based on configuration
-- Deprecated: Remove after migrating msdfgen
function vendorIncludes(...) 
    filter { "configurations:Debug or configurations:Test" }
        externalincludedirs(mapDirNames(..., "../../vendor/Debug/include"))
    
    filter { "configurations:Release or configurations:Profile" }
        externalincludedirs(mapDirNames(..., "../../vendor/Release/include"))
end

-- Set vendor includes based on configuration
-- Deprecated: Remove after migrating msdfgen
function vendorIncludesExternal(...) 
    filter { "configurations:Debug or configurations:Test" }
        externalincludedirs(mapDirNames(..., "../vendor/Debug/include"))
    
    filter { "configurations:Release or configurations:Profile" }
        externalincludedirs(mapDirNames(..., "../vendor/Release/include"))
end

-- Setup all library directories
function setupLibraryDirectories()
    -- This define is needed for libktx to reference
    -- static library functions instead of DLL
    defines {
        "KHRONOS_STATIC"
    }

    filter { "system:windows" }
        externalincludedirs {
            "../vcpkg_installed/x64-windows-static/include",
        }
    filter { "system:linux" }
        externalincludedirs {
            "../vcpkg_installed/x64-linux/include",
        }
    filter {}

    vendorIncludesExternal {
        "",
        "msdfgen"
    }


    filter { "system:windows", "configurations:Debug or configurations:Test" }
        libdirs { "../vendor/Debug/lib", "../vcpkg_installed/x64-windows-static/debug/lib" }
    
    filter { "system:windows", "configurations:Release or configurations:Profile" }
        libdirs { "../vendor/Release/lib", "../vcpkg_installed/x64-windows-static/lib" }

    filter { "system:linux", "configurations:Debug or configurations:Test" }
        libdirs { "../vendor/Debug/lib", "../vcpkg_installed/x64-linux/debug/lib" }
    
    filter { "system:linux", "configurations:Release or configurations:Profile" }
        libdirs { "../vendor/Release/lib", "../vcpkg_installed/x64-linux/lib" }

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
