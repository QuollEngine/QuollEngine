#include "liquid/core/Base.h"

#include "VulkanTexture.h"
#include "VulkanTextureView.h"
#include "VulkanMapping.h"
#include "VulkanError.h"

namespace liquid::rhi {

VulkanTextureView::VulkanTextureView(const TextureViewDescription &description,
                                     VulkanResourceRegistry &registry,
                                     VulkanDeviceObject &device)
    : mDevice(device) {

  auto it = registry.getTextures().find(description.texture);
  LIQUID_ASSERT(it != registry.getTextures().end(),
                "Texture at handle " +
                    std::to_string(static_cast<uint32_t>(description.texture)) +
                    " not found");

  auto &texture = it->second;

  VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;

  if (texture->getDescription().type == TextureType::Cubemap) {
    imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
  } else if (texture->getDescription().layers > 1) {
    imageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
  } else {
    imageViewType = VK_IMAGE_VIEW_TYPE_2D;
  }

  VkImageViewCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.image = texture->getImage();
  createInfo.format =
      VulkanMapping::getFormat(texture->getDescription().format);
  createInfo.viewType = imageViewType;
  createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.subresourceRange.aspectMask = texture->getImageAspectFlags();
  createInfo.subresourceRange.baseMipLevel = description.level;
  createInfo.subresourceRange.levelCount = description.levelCount;
  createInfo.subresourceRange.baseArrayLayer = description.layer;
  createInfo.subresourceRange.layerCount = description.layerCount;

  checkForVulkanError(
      vkCreateImageView(mDevice, &createInfo, nullptr, &mImageView),
      "Failed to create image view");

  mSampler = texture->getSampler();
}

VulkanTextureView::~VulkanTextureView() {
  if (mImageView) {
    vkDestroyImageView(mDevice, mImageView, nullptr);
  }
}

} // namespace liquid::rhi
