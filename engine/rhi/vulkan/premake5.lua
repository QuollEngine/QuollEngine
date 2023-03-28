project "LiquidRHIVulkan"
    basedir "../../../workspace/rhi-vulkan"
    kind "StaticLib"

    includedirs {
        "include",
        "include/liquid/rhi-vulkan",
        "../base/include/liquid/rhi"
    }

    files {
        "src/*.cpp", 
        "include/**.h"
    }

function linkVulkanRHI()
    links { "LiquidRHIVulkan", "LiquidRHICore", "vendor-libvolk", "vendor-libspirv-reflect" }
    includedirs { "../engine/rhi/vulkan/include" }
end
