#include "core/Base.h"
#include "core/EngineGlobals.h"
#include "window/glfw/GLFWWindow.h"
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "VulkanError.h"

namespace liquid {

VkSurfaceKHR createSurfaceFromWindow(VkInstance instance, GLFWWindow *window) {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  checkForVulkanError(glfwCreateWindowSurface(instance, window->getInstance(),
                                              nullptr, &surface),
                      "Failed to create Vulkan Surface");

  LOG_DEBUG("[Vulkan] Surface created");
  return surface;
}

} // namespace liquid
