#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "VulkanAbstraction.h"
#include "VulkanError.h"

namespace liquid {

VulkanAbstraction::VulkanAbstraction(GLFWWindow *window_,
                                     experimental::VulkanRenderDevice *device_)
    : window(window_), device(device_),
      descriptorManager(device->getVulkanDevice(),
                        device->getResourceRegistry()),
      renderContext(device, descriptorManager, statsManager),
      uploadContext(device) {
  createSwapchain();

  graphEvaluator = std::make_unique<VulkanGraphEvaluator>(
      device, swapchain, registry, device->getResourceRegistry());

  resizeHandler = window->addResizeHandler(
      [this](uint32_t x, uint32_t y) mutable { framebufferResized = true; });
}

VulkanAbstraction::~VulkanAbstraction() {
  window->removeResizeHandler(resizeHandler);

  swapchain.destroy();

  window = nullptr;
}

void VulkanAbstraction::execute(RenderGraph &graph) {
  LIQUID_PROFILE_EVENT("VulkanAbstraction::execute");
  statsManager.resetDrawCalls();

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

  RenderCommandList commandList;

  graphEvaluator->execute(commandList, compiled, graph, imageIdx);
  renderContext.render(commandList);

  device->synchronizeDeletes(registry);

  auto queuePresentResult = renderContext.present(swapchain, imageIdx);

  if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR ||
      queuePresentResult == VK_SUBOPTIMAL_KHR || isFramebufferResized()) {
    recreateSwapchain();
  }
}

void VulkanAbstraction::waitForIdle() {
  vkDeviceWaitIdle(device->getVulkanDevice());
}

void VulkanAbstraction::createSwapchain() {
  swapchain = VulkanSwapchain(window, device, swapchain.getSwapchain());

  LOG_DEBUG("[Vulkan] Swapchain created");
}

void VulkanAbstraction::recreateSwapchain() {
  waitForIdle();
  createSwapchain();

  framebufferResized = false;
  swapchainRecreated = true;

  LOG_DEBUG("[Vulkan] Swapchain recreated");
}

} // namespace liquid
