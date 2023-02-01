#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/window/Window.h"

#include "VulkanHeaders.h"
#include <GLFW/glfw3.h>

#include "VulkanError.h"

namespace liquid::rhi {

VkSurfaceKHR createSurfaceFromWindow(VkInstance instance, Window &window) {
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  auto result = glfwCreateWindowSurface(instance, window.getInstance(), nullptr,
                                        &surface);

  if (result == VK_ERROR_INITIALIZATION_FAILED) {
    const char *errorMsg = nullptr;
    if (glfwGetError(&errorMsg) != GLFW_NO_ERROR) {
      checkForVulkanError(result, errorMsg);
    }
  }

  checkForVulkanError(result, "Failed to create window surface");

  return surface;
}

} // namespace liquid::rhi
