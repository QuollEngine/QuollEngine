#include "core/Base.h"
#include "core/EngineGlobals.h"
#include "VulkanRenderBackend.h"
#include "VulkanError.h"

namespace liquid {

VulkanRenderBackend::VulkanRenderBackend(GLFWWindow *window_,
                                         bool enableValidations_,
                                         StatsManager &statsManager)
    : window(window_), vulkanInstance(window_, enableValidations_),
      renderContext(vulkanInstance), uploadContext(vulkanInstance) {
  resourceAllocator = VulkanResourceAllocator::create(
      vulkanInstance, uploadContext, statsManager);

  createSwapchain();

  graphEvaluator = std::make_unique<VulkanGraphEvaluator>(
      vulkanInstance, swapchain, resourceAllocator);

  resizeHandler = window->addResizeHandler(
      [this](uint32_t x, uint32_t y) mutable { framebufferResized = true; });
}

VulkanRenderBackend::~VulkanRenderBackend() {
  window->removeResizeHandler(resizeHandler);

  swapchain.destroy();

  if (resourceAllocator) {
    delete resourceAllocator;
  }

  window = nullptr;
}

void VulkanRenderBackend::execute(RenderGraph &graph) {
  auto &&result = graphEvaluator->build(graph);

  uint32_t imageIdx =
      swapchain.acquireNextImage(renderContext.getImageAvailableSemaphore());

  if (imageIdx == std::numeric_limits<uint32_t>::max()) {
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

void VulkanRenderBackend::waitForIdle() {
  vkDeviceWaitIdle(vulkanInstance.getDevice());
}

void VulkanRenderBackend::createSwapchain() {
  swapchain = VulkanSwapchain(window, vulkanInstance,
                              resourceAllocator->getVmaAllocator(),
                              swapchain.getSwapchain());

  LOG_DEBUG("[Vulkan] Swapchain created");
}

void VulkanRenderBackend::recreateSwapchain() {
  waitForIdle();
  createSwapchain();

  framebufferResized = false;

  LOG_DEBUG("[Vulkan] Swapchain recreated");
}

} // namespace liquid
