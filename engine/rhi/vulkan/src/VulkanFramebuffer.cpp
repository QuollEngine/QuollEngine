#include "liquid/core/Base.h"

#include "VulkanTexture.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanError.h"

namespace liquid::rhi {

VulkanFramebuffer::VulkanFramebuffer(const FramebufferDescription &description,
                                     VulkanDeviceObject &device,
                                     const VulkanResourceRegistry &registry)
    : mDevice(device) {
  std::vector<VkImageView> attachments(description.attachments.size(),
                                       VK_NULL_HANDLE);

  for (size_t i = 0; i < attachments.size(); ++i) {
    attachments.at(i) = registry.getTextures()
                            .at(description.attachments.at(i))
                            ->getImageView();
  }

  VkFramebufferCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  createInfo.flags = 0;
  createInfo.pNext = nullptr;
  createInfo.pAttachments = attachments.data();
  createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  createInfo.renderPass =
      registry.getRenderPasses().at(description.renderPass)->getRenderPass();
  createInfo.width = description.width;
  createInfo.height = description.height;
  createInfo.layers = description.layers;

  checkForVulkanError(
      vkCreateFramebuffer(mDevice, &createInfo, nullptr, &mFramebuffer),
      "Failed to create framebuffer", description.debugName);

  mDevice.setObjectName(description.debugName, VK_OBJECT_TYPE_FRAMEBUFFER,
                        mFramebuffer);
}

VulkanFramebuffer::~VulkanFramebuffer() {
  vkDestroyFramebuffer(mDevice, mFramebuffer, nullptr);
}

} // namespace liquid::rhi
