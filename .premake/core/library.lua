function mapDirNames(arr, basePath)
    local t = {}
    for k, v in pairs(arr) do
        t[k] = basePath .. "/" .. v
    end

    return t
end

-- Set vendor includes based on configuration
function vendorIncludes(...) 
    filter { "configurations:Debug or configurations:Test" }
        externalincludedirs(mapDirNames(..., "../../vendor/Debug/include"))
    
    filter { "configurations:Release or configurations:Profile" }
        externalincludedirs(mapDirNames(..., "../../vendor/Release/include"))
end

-- Set vendor includes based on configuration
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

    vendorIncludesExternal {
        "",
        "freetype2",
        "msdfgen"
    }


    filter { "configurations:Debug or configurations:Test" }
        libdirs { "../vendor/Debug/lib", "../vendor/Debug/lib/debug" }
    
    filter { "configurations:Release or configurations:Profile" }
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
