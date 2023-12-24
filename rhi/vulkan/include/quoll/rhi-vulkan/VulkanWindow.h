#pragma once

#include "VulkanHeaders.h"
#include "quoll/window/Window.h"

namespace quoll::rhi {

/**
 * @brief Create Vulkan surface from window
 *
 * @param instance Vulkan instance
 * @param window Window
 * @return Vulkan surface
 */
VkSurfaceKHR createSurfaceFromWindow(VkInstance instance, Window &window);

} // namespace quoll::rhi
