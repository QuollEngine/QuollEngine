#include "liquid/core/Base.h"
#include "VulkanWindowExtensions.h"

namespace liquid {

#if defined(LIQUID_PLATFORM_WINDOWS)
const std::vector<String> vulkanWindowExtensions{"VK_KHR_surface",
                                                 "VK_KHR_win32_surface"};
#elif defined(LIQUID_PLATFORM_MACOS)
const std::vector<String> vulkanWindowExtensions{
    "VK_EXT_metal_surface", "VK_MVK_macos_surface", "VK_KHR_surface"};
#elif defined(LIQUID_PLATFORM_LINUX)
const std::vector<String> vulkanWindowExtensions{};
#else
const std::vector<String> vulkanWindowExtensions{};
#endif

} // namespace liquid
