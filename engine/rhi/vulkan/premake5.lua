project "QuollRHIVulkan"
    basedir "../../../workspace/rhi-vulkan"
    kind "StaticLib"

    includedirs {
        "include",
        "include/quoll/rhi-vulkan",
        "../base/include/quoll/rhi"
    }

    files {
        "src/*.cpp", 
        "include/**.h"
    }

function linkVulkanRHI()
    links { "QuollRHIVulkan", "QuollRHICore", "volk", "spirv-reflect" }
    includedirs { "../engine/rhi/vulkan/include" }
end
