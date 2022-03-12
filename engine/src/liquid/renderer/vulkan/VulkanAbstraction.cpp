#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "VulkanAbstraction.h"
#include "VulkanError.h"

namespace liquid {

VulkanAbstraction::VulkanAbstraction(GLFWWindow *window_,
                                     experimental::VulkanRenderDevice *device_)
    : window(window_), device(device_),
      swapchain(device->getBackend(), device->getPhysicalDevice(),
                device->getVulkanDevice(), window->getFramebufferSize(),
                VK_NULL_HANDLE) {

  graphEvaluator = std::make_unique<VulkanGraphEvaluator>(
      device, swapchain, registry, device->getResourceRegistry());

  resizeHandler = window->addResizeHandler(
      [this](uint32_t x, uint32_t y) mutable { framebufferResized = true; });
}

VulkanAbstraction::~VulkanAbstraction() {
  window->removeResizeHandler(resizeHandler);

  window = nullptr;
}

void VulkanAbstraction::execute(RenderGraph &graph) {
  LIQUID_PROFILE_EVENT("VulkanAbstraction::execute");
  statsManager.resetDrawCalls();

  auto &renderContext = device->getRenderContext();

  uint32_t imageIdx =
      swapchain.acquireNextImage(renderContext.getImageAvailableSemaphore());

  if (imageIdx == std::numeric_limits<uint32_t>::max()) {
    recreateSwapchain();
    return;
  }

  auto &&compiled = graphEvaluator->compile(graph, swapchainRecreated);
  device->synchronize(registry);

  graphEvaluator->build(compiled, graph, swapchainRecreated);

  if (swapchainRecreated) {
    swapchainRecreated = false;
  }

  auto &commandBuffer = renderContext.beginRendering();
  graphEvaluator->execute(commandBuffer, compiled, graph, imageIdx);
  renderContext.endRendering();

  auto queuePresentResult = renderContext.present(swapchain, imageIdx);
  device->synchronizeDeletes(registry);

  if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR ||
      queuePresentResult == VK_SUBOPTIMAL_KHR || isFramebufferResized()) {
    recreateSwapchain();
  }
}

void VulkanAbstraction::waitForIdle() { device->wait(); }

void VulkanAbstraction::recreateSwapchain() {
  waitForIdle();
  swapchain = experimental::VulkanSwapchain(
      device->getBackend(), device->getPhysicalDevice(),
      device->getVulkanDevice(), window->getFramebufferSize(),
      swapchain.getSwapchain());

  framebufferResized = false;
  swapchainRecreated = true;

  LOG_DEBUG("[Vulkan] Swapchain recreated");
}

} // namespace liquid