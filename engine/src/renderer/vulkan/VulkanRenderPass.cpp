#include "core/Base.h"
#include "VulkanRenderPass.h"

#include "core/EngineGlobals.h"

namespace liquid {

VulkanRenderPass::VulkanRenderPass(
    VkDevice device_, VkRenderPass renderPass_,
    const std::vector<VkFramebuffer> &framebuffers_,
    const std::vector<VkClearValue> &clearValues_, uint32_t width_,
    uint32_t height_, uint32_t layers_)
    : device(device_), renderPass(renderPass_), framebuffers(framebuffers_),
      clearValues(clearValues_), extent{width_, height_}, layers(layers_) {}

VulkanRenderPass::~VulkanRenderPass() {
  for (auto &x : framebuffers) {
    vkDestroyFramebuffer(device, x, nullptr);
  }
  LOG_DEBUG("[Vulkan] Framebuffers destroyed");

  if (renderPass) {
    vkDestroyRenderPass(device, renderPass, nullptr);
  }

  LOG_DEBUG("[Vulkan] Render pass destroyed");
}

} // namespace liquid
