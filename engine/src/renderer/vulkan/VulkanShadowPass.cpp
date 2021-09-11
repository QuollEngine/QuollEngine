#include "VulkanShadowPass.h"
#include "VulkanError.h"
#include "VulkanTextureBinder.h"
#include "core/EngineGlobals.h"
#include "profiler/StatsManager.h"

namespace liquid {

constexpr uint32_t NUM_LIGHTS = 16;

VulkanShadowPass::VulkanShadowPass(uint32_t shadowmapDimensions,
                                   VkDevice device_,
                                   VulkanPipelineBuilder *pipelineBuilder,
                                   ResourceAllocator *resourceAllocator_,
                                   VulkanDescriptorManager *descriptorManager,
                                   StatsManager &statsManager)
    : shadowmapExtent{shadowmapDimensions, shadowmapDimensions},
      device(device_), resourceAllocator(resourceAllocator_) {
  createRenderPass();
  createResourceManager(pipelineBuilder, descriptorManager);
  createTextures(statsManager);
  createFramebuffers();
}

VulkanShadowPass::~VulkanShadowPass() {
  if (framebuffer) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
    LOG_DEBUG("[Vulkan] Shadow framebuffer destroyed");
  }

  if (renderPass) {
    vkDestroyRenderPass(device, renderPass, nullptr);
    LOG_DEBUG("[Vulkan] Shadow renderPass destroyed");
  }
}

void VulkanShadowPass::render(
    RenderCommandList &commandList,
    std::function<void(RenderCommandList &commandList)> renderFn) {

  VkClearValue clearValue{};
  clearValue.depthStencil.depth = 1.0f;
  clearValue.depthStencil.stencil = 0;

  commandList.beginRenderPass(renderPass, framebuffer, {0, 0},
                              {shadowmapExtent.width, shadowmapExtent.height},
                              {clearValue});

  renderFn(commandList);

  commandList.endRenderPass();
}

void VulkanShadowPass::createRenderPass() {
  VkAttachmentDescription depthAttachment{};
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
  depthAttachment.format = VK_FORMAT_D16_UNORM;

  VkAttachmentReference depthReference{};
  depthReference.attachment = 0;
  depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.colorAttachmentCount = 0;
  subpass.pDepthStencilAttachment = &depthReference;
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  std::array<VkSubpassDependency, 2> subpassDependencies{};
  subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  subpassDependencies[0].dstSubpass = 0;
  subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  subpassDependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
  subpassDependencies[0].dstStageMask =
      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  subpassDependencies[0].dstAccessMask =
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  subpassDependencies[1].srcSubpass = 0;
  subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
  subpassDependencies[1].srcStageMask =
      VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  subpassDependencies[1].srcAccessMask =
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  subpassDependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  VkRenderPassCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  createInfo.flags = 0;
  createInfo.pNext = nullptr;
  createInfo.pSubpasses = &subpass;
  createInfo.subpassCount = 1;
  createInfo.pAttachments = &depthAttachment;
  createInfo.attachmentCount = 1;
  createInfo.pDependencies = subpassDependencies.data();
  createInfo.dependencyCount = subpassDependencies.size();

  checkForVulkanError(
      vkCreateRenderPass(device, &createInfo, nullptr, &renderPass),
      "Failed to create shadow render pass");
  LOG_DEBUG("[Vulkan] Shadow render pass created");
}

void VulkanShadowPass::createResourceManager(
    VulkanPipelineBuilder *pipelineBuilder,
    VulkanDescriptorManager *descriptorManager) {
  resourceManager = std::make_shared<VulkanResourceManager>(
      descriptorManager, pipelineBuilder, renderPass, 0);
}

void VulkanShadowPass::createTextures(StatsManager &statsManager) {

  TextureFramebufferData data{};
  data.width = shadowmapExtent.width;
  data.height = shadowmapExtent.height;
  data.layers = NUM_LIGHTS;
  data.format = VK_FORMAT_D16_UNORM;
  shadowmapTexture = resourceAllocator->createTextureFramebuffer(data);
}

void VulkanShadowPass::createFramebuffers() {
  VkImageView imageView = std::dynamic_pointer_cast<VulkanTextureBinder>(
                              shadowmapTexture->getResourceBinder())
                              ->getImageView();
  std::array<VkImageView, 1> fbAttachments = {imageView};
  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.flags = 0;
  framebufferInfo.pNext = nullptr;
  framebufferInfo.layers = NUM_LIGHTS;
  framebufferInfo.pAttachments = fbAttachments.data();
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.width = shadowmapExtent.width;
  framebufferInfo.height = shadowmapExtent.height;

  checkForVulkanError(
      vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer),
      "Failed to create shadow framebuffer");
  LOG_DEBUG("[Vulkan] Shadow framebuffer created");
}

} // namespace liquid
