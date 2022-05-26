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

  std::vector<VkAttachmentDescription> attachments(
      description.attachments.size());
  mClearValues.resize(attachments.size());

  std::vector<VkAttachmentReference> colorReferences;
  std::optional<VkAttachmentReference> depthReference;

  for (size_t i = 0; i < description.attachments.size(); ++i) {
    auto &attachment = attachments.at(i);
    auto &desc = description.attachments.at(i);
    const auto &texture = registry.getTextures().at(desc.texture);

    attachment.flags = 0;
    attachment.loadOp = VulkanMapping::getAttachmentLoadOp(desc.loadOp);
    attachment.storeOp = VulkanMapping::getAttachmentStoreOp(desc.storeOp);
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.format = texture->getFormat();

    VkAttachmentReference ref;
    ref.attachment = static_cast<uint32_t>(i);

    if ((texture->getDescription().usage & TextureUsage::Color) ==
        TextureUsage::Color) {

      mClearValues.at(i).color.float32[0] =
          std::get<glm::vec4>(desc.clearValue).x;
      mClearValues.at(i).color.float32[1] =
          std::get<glm::vec4>(desc.clearValue).y;
      mClearValues.at(i).color.float32[2] =
          std::get<glm::vec4>(desc.clearValue).z;
      mClearValues.at(i).color.float32[3] =
          std::get<glm::vec4>(desc.clearValue).w;

      attachment.initialLayout = desc.initialLayout;
      attachment.finalLayout = desc.layout != VK_IMAGE_LAYOUT_MAX_ENUM
                                   ? desc.layout
                                   : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      colorReferences.push_back(ref);
    } else if ((texture->getDescription().usage & TextureUsage::Depth) ==
               TextureUsage::Depth) {

      mClearValues.at(i).depthStencil.depth =
          std::get<DepthStencilClear>(desc.clearValue).clearDepth;
      mClearValues.at(i).depthStencil.stencil =
          std::get<DepthStencilClear>(desc.clearValue).clearStencil;

      attachment.initialLayout = desc.initialLayout;
      attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      depthReference.emplace(ref);
    }
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
