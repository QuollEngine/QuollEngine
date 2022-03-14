#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"

#include "VulkanRenderPass.h"
#include "VulkanTexture.h"
#include "VulkanMapping.h"
#include "VulkanError.h"

namespace liquid::rhi {

VulkanRenderPass::VulkanRenderPass(const RenderPassDescription &description,
                                   VulkanDeviceObject &device,
                                   const VulkanResourceRegistry &registry)
    : mDevice(device) {

  std::vector<VkAttachmentReference> colorReferences(
      description.colorAttachments.size());
  VkAttachmentReference depthReference;

  bool hasDepthAttachment = description.depthAttachment.texture > 0;

  std::vector<VkAttachmentDescription> attachments(
      description.colorAttachments.size() + (hasDepthAttachment ? 1 : 0));

  for (size_t i = 0; i < description.colorAttachments.size(); ++i) {
    auto &attachment = attachments.at(i);
    auto &desc = description.colorAttachments.at(i);
    auto &ref = colorReferences.at(i);

    attachment.flags = 0;
    attachment.loadOp = VulkanMapping::getAttachmentLoadOp(desc.loadOp);
    attachment.storeOp = VulkanMapping::getAttachmentStoreOp(desc.storeOp);
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.format = registry.getTextures().at(desc.texture)->getFormat();
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = desc.layout != VK_IMAGE_LAYOUT_MAX_ENUM
                                 ? desc.layout
                                 : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    ref.attachment = static_cast<uint32_t>(i);
  }

  if (hasDepthAttachment) {
    auto &attachment = attachments.at(attachments.size() - 1);
    auto &desc = description.depthAttachment;

    attachment.flags = 0;
    attachment.loadOp = VulkanMapping::getAttachmentLoadOp(desc.loadOp);
    attachment.storeOp = VulkanMapping::getAttachmentStoreOp(desc.storeOp);
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.format = registry.getTextures().at(desc.texture)->getFormat();
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthReference.attachment = static_cast<uint32_t>(attachments.size() - 1);
  }

  VkSubpassDescription subpass{};
  subpass.flags = 0;
  subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
  subpass.pColorAttachments = colorReferences.data();
  subpass.pDepthStencilAttachment =
      hasDepthAttachment ? &depthReference : nullptr;
  subpass.inputAttachmentCount = 0;
  subpass.pInputAttachments = nullptr;
  subpass.preserveAttachmentCount = 0;
  subpass.pPreserveAttachments = nullptr;
  subpass.pipelineBindPoint = description.bindPoint;
  subpass.pResolveAttachments = nullptr;

  VkRenderPassCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  createInfo.flags = 0;
  createInfo.pNext = nullptr;
  createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  createInfo.pAttachments = attachments.data();
  createInfo.subpassCount = 1;
  createInfo.pSubpasses = &subpass;
  createInfo.dependencyCount = 0;
  createInfo.pDependencies = nullptr;

  checkForVulkanError(
      vkCreateRenderPass(mDevice, &createInfo, nullptr, &mRenderPass),
      "Failed to create render pass");

  LOG_DEBUG("[Vulkan] Render pass created");
}

VulkanRenderPass::~VulkanRenderPass() {
  vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

  LOG_DEBUG("[Vulkan] Render pass destroyed");
}

} // namespace liquid::rhi
