#include "quoll/core/Base.h"
#include "VulkanError.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanTexture.h"

namespace quoll::rhi {

VulkanFramebuffer::VulkanFramebuffer(const FramebufferDescription &description,
                                     VulkanDeviceObject &device,
                                     const VulkanResourceRegistry &registry)
    : mDevice(device) {
  std::vector<VkImageView> attachments(description.attachments.size(),
                                       VK_NULL_HANDLE);

  for (usize i = 0; i < attachments.size(); ++i) {
    attachments.at(i) = registry.getTextures()
                            .at(description.attachments.at(i))
                            ->getImageView();
  }

  VkFramebufferCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  createInfo.flags = 0;
  createInfo.pNext = nullptr;
  createInfo.pAttachments = attachments.data();
  createInfo.attachmentCount = static_cast<u32>(attachments.size());
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

} // namespace quoll::rhi
