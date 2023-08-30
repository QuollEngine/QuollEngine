#include "quoll/core/Base.h"

#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanError.h"
#include "VulkanMapping.h"

namespace quoll::rhi {

VulkanTexture::VulkanTexture(VkImage image, VkImageView imageView,
                             VkSampler sampler, VkFormat format,
                             VulkanResourceAllocator &allocator,
                             VulkanDeviceObject &device)
    : mAllocator(allocator), mDevice(device), mImage(image),
      mImageView(imageView), mFormat(format) {

  // Note: This constructor is ONLY used
  // for defining swapchain; so, its usage
  // will always be Color
  mDescription.usage = TextureUsage::Color;
}

VulkanTexture::VulkanTexture(const TextureDescription &description,
                             VulkanResourceAllocator &allocator,
                             VulkanDeviceObject &device)
    : mAllocator(allocator), mDevice(device),
      mFormat(VulkanMapping::getFormat(description.format)),
      mDescription(description) {

  QuollAssert(description.width > 0 && description.height > 0,
              "Texture dimensions cannot be zero");

  QuollAssert(description.type == TextureType::Cubemap
                  ? description.layerCount == 6
                  : true,
              "Cubemap must have 6 layers");

  uint32_t imageFlags = description.type == TextureType::Cubemap
                            ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
                            : 0;

  VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;

  if (description.type == TextureType::Cubemap) {
    imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
  } else if (description.layerCount > 1) {
    imageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
  } else {
    imageViewType = VK_IMAGE_VIEW_TYPE_2D;
  }

  static constexpr uint32_t HundredPercent = 100;

  VkExtent3D extent{};
  extent.width = description.width;
  extent.height = description.height;
  extent.depth = description.depth;

  VkImageUsageFlags usageFlags = 0;

  if (BitwiseEnumContains(description.usage, TextureUsage::Color)) {
    usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    mAspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
  }

  if (BitwiseEnumContains(description.usage, TextureUsage::Depth)) {
    usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    mAspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
  }

  if (BitwiseEnumContains(description.usage, TextureUsage::Stencil)) {
    usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    mAspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
  }

  if (BitwiseEnumContains(description.usage, TextureUsage::Sampled)) {
    usageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
  }

  if (BitwiseEnumContains(description.usage, TextureUsage::TransferSource)) {
    usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }

  if (BitwiseEnumContains(description.usage,
                          TextureUsage::TransferDestination)) {
    usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

  if (BitwiseEnumContains(description.usage, TextureUsage::Storage)) {
    usageFlags |= VK_IMAGE_USAGE_STORAGE_BIT;
  }

  VkImageCreateInfo imageCreateInfo{};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.pNext = nullptr;
  imageCreateInfo.flags = imageFlags;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = mFormat;
  imageCreateInfo.extent = extent;
  imageCreateInfo.mipLevels = description.mipLevelCount;
  imageCreateInfo.arrayLayers = description.layerCount;
  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.usage = usageFlags;
  imageCreateInfo.samples =
      static_cast<VkSampleCountFlagBits>(description.samples);

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
  allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
  checkForVulkanError(vmaCreateImage(mAllocator, &imageCreateInfo,
                                     &allocationCreateInfo, &mImage,
                                     &mAllocation, nullptr),
                      "Failed to create texture", description.debugName);

  mDevice.setObjectName(description.debugName, VK_OBJECT_TYPE_IMAGE, mImage);

  mViewDescription.layerCount = description.layerCount;
  mViewDescription.mipLevelCount = description.mipLevelCount;

  VkImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewCreateInfo.pNext = nullptr;
  imageViewCreateInfo.flags = 0;
  imageViewCreateInfo.image = mImage;
  imageViewCreateInfo.viewType = imageViewType;
  imageViewCreateInfo.format = mFormat;
  imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
  imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
  imageViewCreateInfo.subresourceRange.layerCount = description.layerCount;
  imageViewCreateInfo.subresourceRange.levelCount = description.mipLevelCount;
  imageViewCreateInfo.subresourceRange.aspectMask = mAspectFlags;
  checkForVulkanError(
      vkCreateImageView(mDevice, &imageViewCreateInfo, nullptr, &mImageView),
      "Failed to create image view", description.debugName);

  mDevice.setObjectName(description.debugName, VK_OBJECT_TYPE_IMAGE_VIEW,
                        mImageView);
}

VulkanTexture::VulkanTexture(const TextureViewDescription &description,
                             VulkanResourceRegistry &registry,
                             VulkanResourceAllocator &allocator,
                             VulkanDeviceObject &device)
    : mDevice(device), mAllocator(allocator) {
  const auto &original = registry.getTextures().at(description.texture);

  mImage = original->mImage;
  mFormat = original->mFormat;
  mAspectFlags = original->mAspectFlags;
  mDescription = original->mDescription;
  mViewDescription = description;

  VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
  if (original->mDescription.type == TextureType::Cubemap) {
    imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
  } else if (description.layerCount > 1) {
    imageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
  } else {
    imageViewType = VK_IMAGE_VIEW_TYPE_2D;
  }

  VkImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewCreateInfo.pNext = nullptr;
  imageViewCreateInfo.flags = 0;
  imageViewCreateInfo.image = mImage;
  imageViewCreateInfo.viewType = imageViewType;
  imageViewCreateInfo.format = original->mFormat;
  imageViewCreateInfo.subresourceRange.baseMipLevel = description.baseMipLevel;
  imageViewCreateInfo.subresourceRange.levelCount = description.mipLevelCount;
  imageViewCreateInfo.subresourceRange.baseArrayLayer = description.baseLayer;
  imageViewCreateInfo.subresourceRange.layerCount = description.layerCount;
  imageViewCreateInfo.subresourceRange.aspectMask = mAspectFlags;

  checkForVulkanError(
      vkCreateImageView(mDevice, &imageViewCreateInfo, nullptr, &mImageView),
      "Failed to create image view", description.debugName);

  mDevice.setObjectName(description.debugName, VK_OBJECT_TYPE_IMAGE_VIEW,
                        mImageView);
}

VulkanTexture::~VulkanTexture() {
  if (mImageView) {
    vkDestroyImageView(mDevice, mImageView, nullptr);
  }

  if (mAllocation && mImage) {
    vmaDestroyImage(mAllocator, mImage, mAllocation);
  }
}

} // namespace quoll::rhi
