#pragma once

#include <vulkan/vulkan.hpp>
#include "liquid/window/glfw/GLFWWindow.h"

namespace liquid {

/**
 * @brief Create Vulkan surface from window
 *
 * @param instance Vulkan instance
 * @param window Window
 * @return Vulkan surface
 */
VkSurfaceKHR createSurfaceFromWindow(VkInstance instance, GLFWWindow &window);

} // namespace liquid
