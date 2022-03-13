#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "liquid/window/Window.h"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "VulkanError.h"

namespace liquid {

VkSurfaceKHR createSurfaceFromWindow(VkInstance instance, Window &window) {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  checkForVulkanError(glfwCreateWindowSurface(instance, window.getInstance(),
                                              nullptr, &surface),
                      "Failed to create Vulkan Surface");

  LOG_DEBUG("[Vulkan] Surface created");
  return surface;
}

} // namespace liquid
