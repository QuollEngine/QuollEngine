#include "liquid/core/Base.h"
#include "liquid/window/glfw/GLFWWindow.h"
#include "liquid/rhi/vulkan/VulkanRenderBackend.h"

int main() {
  liquid::GLFWWindow window("RHI Test", 800, 600);
  liquid::experimental::VulkanRenderBackend backend(window);
  auto *device = backend.getOrCreateDevice();

  while (!window.shouldClose()) {
    window.pollEvents();
  }

  return 0;
}