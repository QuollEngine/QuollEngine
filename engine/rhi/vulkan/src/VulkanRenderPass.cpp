#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "VulkanRenderPass.h"
#include "VulkanTexture.h"
#include "VulkanMapping.h"
#include "VulkanError.h"
#include "VulkanLog.h"

namespace liquid::rhi {

VulkanRenderPass::VulkanRenderPass(const RenderPassDescription &description,
                                   VulkanDeviceObject &device,
                                   const VulkanResourceRegistry &registry)
    : mDevice(device) {

  size_t numDepthAttachments = description.depthAttachment.has_value() ? 1 : 0;
  size_t numResolveAttachments =
      description.resolveAttachment.has_value() ? 1 : 0;

  size_t numAttachments = description.colorAttachments.size() +
                          numDepthAttachments + numResolveAttachments;

  std::vector<VkAttachmentDescription> attachments;
  attachments.reserve(numAttachments);
  mClearValues.reserve(numAttachments);

  std::vector<VkAttachmentReference> colorReferences;
  std::optional<VkAttachmentReference> depthReference;
  std::optional<VkAttachmentReference> resolveReference;

  for (size_t i = 0; i < description.colorAttachments.size(); ++i) {
    auto &desc = description.colorAttachments.at(i);
    const auto &texture = registry.getTextures().at(desc.texture);

    LIQUID_ASSERT(BitwiseEnumContains(texture->getDescription().usage,
                                      TextureUsage::Color),
                  "Texture cannot be a color attachment");

    VkAttachmentDescription attachment =
        getVulkanAttachmentDescription(desc, registry);
    attachment.initialLayout =
        VulkanMapping::getImageLayout(desc.initialLayout);
    attachment.finalLayout = VulkanMapping::getImageLayout(desc.layout);
    attachments.push_back(attachment);

    VkAttachmentReference ref;
    ref.attachment = static_cast<uint32_t>(attachments.size() - 1);
    ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorReferences.push_back(ref);

    VkClearValue clearValue;
    clearValue.color.float32[0] = std::get<glm::vec4>(desc.clearValue).x;
    clearValue.color.float32[1] = std::get<glm::vec4>(desc.clearValue).y;
    clearValue.color.float32[2] = std::get<glm::vec4>(desc.clearValue).z;
    clearValue.color.float32[3] = std::get<glm::vec4>(desc.clearValue).w;
    mClearValues.push_back(clearValue);
  }

  if (description.depthAttachment.has_value()) {
    const auto &desc = description.depthAttachment.value();
    const auto &texture = registry.getTextures().at(desc.texture);

    LIQUID_ASSERT(BitwiseEnumContains(texture->getDescription().usage,
                                      TextureUsage::Depth),
                  "Texture cannot be a depth attachment");

    VkAttachmentDescription attachment =
        getVulkanAttachmentDescription(desc, registry);
    attachment.initialLayout =
        VulkanMapping::getImageLayout(desc.initialLayout);
    attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments.push_back(attachment);

    VkAttachmentReference ref;
    ref.attachment = static_cast<uint32_t>(attachments.size() - 1);
    ref.layout = attachment.finalLayout;
    depthReference.emplace(ref);

    VkClearValue clearValue;
    clearValue.depthStencil.depth =
        std::get<DepthStencilClear>(desc.clearValue).clearDepth;
    clearValue.depthStencil.stencil =
        std::get<DepthStencilClear>(desc.clearValue).clearStencil;
    mClearValues.push_back(clearValue);
  }

  if (description.resolveAttachment.has_value()) {
    const auto &desc = description.resolveAttachment.value();

    VkAttachmentDescription attachment =
        getVulkanAttachmentDescription(desc, registry);

    attachment.initialLayout =
        VulkanMapping::getImageLayout(desc.initialLayout);
    attachment.finalLayout = VulkanMapping::getImageLayout(desc.layout);

    attachments.push_back(attachment);

    VkAttachmentReference ref;
    ref.attachment = static_cast<uint32_t>(attachments.size() - 1);
    ref.layout = attachment.finalLayout;
    resolveReference.emplace(ref);

    VkClearValue clearValue;
    clearValue.color.float32[0] = std::get<glm::vec4>(desc.clearValue).x;
    clearValue.color.float32[1] = std::get<glm::vec4>(desc.clearValue).y;
    clearValue.color.float32[2] = std::get<glm::vec4>(desc.clearValue).z;
    clearValue.color.float32[3] = std::get<glm::vec4>(desc.clearValue).w;
    mClearValues.push_back(clearValue);
  }

  VkSubpassDescription subpass{};
  subpass.flags = 0;
  subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
  subpass.pColorAttachments = colorReferences.data();
  subpass.pDepthStencilAttachment =
      depthReference.has_value() ? &depthReference.value() : nullptr;
  subpass.inputAttachmentCount = 0;
  subpass.pInputAttachments = nullptr;
  subpass.preserveAttachmentCount = 0;
  subpass.pPreserveAttachments = nullptr;
  subpass.pipelineBindPoint =
      VulkanMapping::getPipelineBindPoint(description.bindPoint);
  subpass.pResolveAttachments =
      resolveReference.has_value() ? &resolveReference.value() : nullptr;

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

  LOG_DEBUG_VK("Render pass created", mRenderPass);
}

VulkanRenderPass::~VulkanRenderPass() {
  vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

  LOG_DEBUG_VK("Render pass destroyed", mRenderPass);
}

VkAttachmentDescription VulkanRenderPass::getVulkanAttachmentDescription(
    const RenderPassAttachmentDescription &description,
    const VulkanResourceRegistry &registry) {
  const auto &texture = registry.getTextures().at(description.texture);

  VkAttachmentDescription attachment{};
  attachment.flags = 0;
  attachment.loadOp = VulkanMapping::getAttachmentLoadOp(description.loadOp);
  attachment.storeOp = VulkanMapping::getAttachmentStoreOp(description.storeOp);
  attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachment.samples =
      static_cast<VkSampleCountFlagBits>(texture->getDescription().samples);
  attachment.format = texture->getFormat();

  return attachment;
}

} // namespace liquid::rhi
