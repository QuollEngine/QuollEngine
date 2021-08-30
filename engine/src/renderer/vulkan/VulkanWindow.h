#pragma once

#include <vulkan/vulkan.hpp>
#include "window/glfw/GLFWWindow.h"
#include "VulkanError.h"

namespace liquid {

/**
 * @brief Create Vulkan surface from window
 *
 * @param instance Vulkan instance
 * @param window Pointer to window
 * @return Vulkan surface
 */
VkSurfaceKHR createSurfaceFromWindow(VkInstance instance, GLFWWindow *window);

} // namespace liquid
