#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "VulkanAbstraction.h"
#include "VulkanError.h"

namespace liquid {

VulkanAbstraction::VulkanAbstraction(GLFWWindow *window_,
                                     experimental::VulkanRenderDevice *device_)
    : window(window_), device(device_),
      descriptorManager(device->getVulkanDevice()),
      renderContext(device, descriptorManager, statsManager),
      uploadContext(device) {
  resourceAllocator =
      VulkanResourceAllocator::create(device, uploadContext, statsManager);

  createSwapchain();

  graphEvaluator = std::make_unique<VulkanGraphEvaluator>(device, swapchain,
                                                          resourceAllocator);

  resizeHandler = window->addResizeHandler(
      [this](uint32_t x, uint32_t y) mutable { framebufferResized = true; });
}

VulkanAbstraction::~VulkanAbstraction() {
  window->removeResizeHandler(resizeHandler);

  swapchain.destroy();

  if (resourceAllocator) {
    delete resourceAllocator;
  }

  window = nullptr;
}

void VulkanAbstraction::execute(RenderGraph &graph) {
  LIQUID_PROFILE_EVENT("VulkanAbstraction::execute");
  statsManager.resetDrawCalls();
  auto &&result = graphEvaluator->build(graph);

  uint32_t imageIdx =
      swapchain.acquireNextImage(renderContext.getImageAvailableSemaphore());

  if (imageIdx == std::numeric_limits<uint32_t>::max()) {
    recreateSwapchain();
    graphEvaluator->rebuildSwapchainRelatedPasses(graph);
    return;
  }

  RenderCommandList commandList;

  graphEvaluator->execute(commandList, result, graph, imageIdx);

  renderContext.render(commandList);

  auto queuePresentResult = renderContext.present(swapchain, imageIdx);

  if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR ||
      queuePresentResult == VK_SUBOPTIMAL_KHR || isFramebufferResized()) {
    recreateSwapchain();
    graphEvaluator->rebuildSwapchainRelatedPasses(graph);
  }
}

void VulkanAbstraction::waitForIdle() {
  vkDeviceWaitIdle(device->getVulkanDevice());
}

void VulkanAbstraction::createSwapchain() {
  swapchain =
      VulkanSwapchain(window, device, resourceAllocator->getVmaAllocator(),
                      swapchain.getSwapchain());

  LOG_DEBUG("[Vulkan] Swapchain created");
}

void VulkanAbstraction::recreateSwapchain() {
  waitForIdle();
  createSwapchain();

  framebufferResized = false;

  LOG_DEBUG("[Vulkan] Swapchain recreated");
}

} // namespace liquid
